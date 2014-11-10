#include "main.h"

#include "alarm.h"
#include "file.h"
#include "options.h"
#include "time.h"

#include <Windows.h>

#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MILLISECOND -10000LL
#define SECOND -10000000LL

static void
print_help()
{
	fputs("Usage: alarm activate\n\n"
		"Activates all set alarms.\n", stdout);
}

static void
print_version()
{
	fputs("alarm activate - version 1.0\n"
		"last updated 2014-10-03\n", stdout);
}

LRESULT CALLBACK wnd_proc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
	return DefWindowProcA(wnd, msg, wp, lp);
}

static int
alarm_visual(const Alarm *alarm, bool *stop)
{
	HBRUSH brush;
	WNDCLASSA wc;
	HWND wnd;
	AlarmLevel level;

	ASSUME(alarm != NULL);
	ASSUME(stop != NULL);

	level = alarm->options().level;

	brush = CreateSolidBrush(level == AlarmLevel::CRITICAL ?
		RGB(255, 0, 0) :
		level == AlarmLevel::WARNING ?
			RGB(255, 255, 0) :
			RGB(0, 255, 0));
	if (brush == NULL) {
		fputs("Error: unable to create Windows brush\n", stderr);
		return -1;
	}

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = brush;
	wc.hCursor = NULL;
	wc.lpfnWndProc = &wnd_proc;
	wc.hIcon = NULL;
	wc.hInstance = NULL;
	wc.lpszClassName = "AlarmVisual";
	wc.lpszMenuName = NULL;
	wc.style = NULL;
	if (!RegisterClassA(&wc)) {
		DeleteObject(brush);

		fputs("Error: unable to register Windows class\n", stderr);

		return -1;
	}

	wnd = CreateWindowA("AlarmVisual", NULL, WS_POPUP, 0, 0,
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		NULL, NULL, NULL, NULL);
	if (wnd == NULL) {
		UnregisterClassA("AlarmVisual", NULL);
		DeleteObject(brush);
		
		fputs("Error: unable to create Windows window\n", stderr);

		return -1;
	}

	ShowWindow(wnd, SW_SHOWNORMAL);
	ShowWindow(wnd, SW_HIDE);

	do {
		ShowWindow(wnd, SW_SHOW);

		std::this_thread::sleep_for(std::chrono::milliseconds(300));

		ShowWindow(wnd, SW_HIDE);

		std::this_thread::sleep_for(std::chrono::milliseconds(700));
	} while (!*stop);

	DestroyWindow(wnd);
	UnregisterClassA("AlarmVisual", NULL);
	DeleteObject(brush);

	return 0;
}

static int
alarm_sound(const Alarm *alarm, bool *stop)
{
	AlarmLevel level;
	int period;

	ASSUME(alarm != NULL);
	ASSUME(stop != NULL);

	level = alarm->options().level;

	period = level == AlarmLevel::CRITICAL ?
		300 :
		level == AlarmLevel::WARNING ?
			500 :
			1000;

	do {
		if (!Beep(988, period * 3 / 10)) {
			return -1;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(period * 7 / 10));
	} while (!*stop);

	return 0;
}

static int
set_timer(Alarm &);

static inline int
raise_alarm(Alarm &alarm)
{
	unsigned int err;
	std::thread sound, visual;
	bool stop = false;

	err = 0;

	if (alarm.options().visual) {
		try {
			visual = std::thread(&alarm_visual, &alarm, &stop);
		}
		catch (...) {
			fputs("Error: unable to use visuals\n", stderr);

			err |= 1;
		}
	}

	if (alarm.options().sound) {
		try {
			sound = std::thread(&alarm_sound, &alarm, &stop);
		}
		catch (...) {
			fputs("Error: unable to use sounds\n", stderr);

			err |= 2;
		}
	}

	if ((err & 1) && (err & 2)) {
		return -1;
	}

	for (;;) {
		int i, r1, r2;

		r1 = rand();
		r2 = rand();

		if (r1 > r2) {
			std::swap(r1, r2);
		}

		if (alarm.options().sleep) {
			fprintf(stdout,
				"To sleep this alarm, type the number %i and press enter.\n",
				r1);
		}
		else {
			fputs("Sleep is disabled for this alarm.\n", stdout);
		}

		fprintf(stdout,
			"To stop this alarm, type the number %i and press enter.\n",
			r2);

		if (fscanf(stdin, "%i", &i) != 1) {
			fputc('\n', stdout);
			continue;
		}

		if (i == r1) {
			stop = true;

			alarm.sleep();

			try {
				if (sound.joinable()) {
					sound.join();
				}

				if (visual.joinable()) {
					visual.join();
				}
			}
			catch (...) {
				fputs("Error: unable to join threads\n", stderr);
				return -1;
			}

			return set_timer(alarm);
		}

		if (i == r2) {
			stop = true;

			try {
				if (sound.joinable()) {
					sound.join();
				}

				if (visual.joinable()) {
					visual.join();
				}
			}
			catch (...) {
				fputs("Error: unable to join threads\n", stderr);
				return -1;
			}

			return 0;
		}
	}

	ASSUME_UNREACHABLE();
}

static inline LARGE_INTEGER
get_due_time(const AlarmTime &time)
{
	LARGE_INTEGER l;
	AlarmTime now, diff;

	if (now.get_now() != 0) {
		l.QuadPart = 1LL;
		return l;
	}

#define DIFF(p) diff.p = time.p - now.p

	DIFF(year);
	DIFF(month);
	DIFF(day);
	DIFF(hour);
	DIFF(minute);
	DIFF(second);

#undef DIFF

	if (diff.month != 0 || diff.year != 0) {
		TODO(Make the alarm work across months; otherwise it may be difficult to get up on the 1st.);

		fputs("Error: setting alarms across months is not currently supported\n", stderr);

		l.QuadPart = 1LL;
		return l;
	}

	l.QuadPart = SECOND * (int)diff.second + SECOND * 60 * (int)diff.minute
		+ SECOND * 60 * 60 * (int)diff.hour
		+ SECOND * 60 * 60 * 24 * (int)diff.day;

	return l;
}

static int
set_timer(Alarm &alarm)
{
	HANDLE timer;
	LARGE_INTEGER l;

	l = get_due_time(alarm.time());
	if (l.QuadPart == 1LL) {
		return -1;
	}

	timer = CreateWaitableTimerA(NULL, FALSE, NULL);

	if (!SetWaitableTimer(timer, &l, 0, NULL, NULL,
		alarm.options().sleep ? TRUE : FALSE)) {
		CloseHandle(timer);

		fputs("Error: unable to create timer\n", stderr);

		return -1;
	}

	if (WaitForSingleObject(timer, INFINITE) != WAIT_OBJECT_0) {
		CloseHandle(timer);
		
		fputs("Error: unable to wait for the timer to finish\n", stderr);

		return -1;
	}

	CloseHandle(timer);

	return 0;
}

static inline int
activate_alarm(Alarm &alarm)
{
	fputs("Activating alarm:\n", stdout);
	alarm.print_human(stdout);

	if (set_timer(alarm) != 0) {
		return -1;
	}

	if (raise_alarm(alarm) != 0) {
		return -1;
	}

	return 0;
}

int
activate_main(int argc, char **argv, const char *argstr)
{
	std::vector<Alarm> alarms;

	if (argc > 0) {
		if (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "-h") == 0) {
			print_help();
			return 0;
		}

		if (strcmp(argv[0], "--version") == 0 || strcmp(argv[0], "-v") == 0) {
			print_version();
			return 0;
		}
	}

	if (read_file(alarms) != 0) {
		return 1;
	}

	do {
		void tidy(std::vector<Alarm> &);
		tidy(alarms);
	} while (0);

	if (alarms.empty()) {
		fputs("There are no alarms set to activate.\n", stdout);
	}
	else {
		for (Alarm &alarm : alarms) {
			AlarmTime now;

			now.get_now();

			if (AlarmTime::cmp(alarm.time(), now) <= 0) {
				continue;
			}

			if (activate_alarm(alarm) != 0) {
				return 1;
			}
		}
	}
	
	return 0;
}