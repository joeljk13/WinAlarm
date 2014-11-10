#include "main.h"

#include "alarm.h"
#include "file.h"
#include "time.h"

#include <algorithm>
#include <vector>

#include <stdio.h>
#include <string.h>

static void
print_help()
{
	fputs("Usage: alarm tidy\n\n"
		"Tidies up the alarms by:\n"
		"\tRemoving old alarms\n"
		"\tRemoving duplicate alarms\n"
		"\t\t(multiple alarms at the same time)\n"
		"\tSorting the remaining alarms from soonest to latest.\n", stdout);
}

static void
print_version()
{
	fputs("alarm tidy - version 1.0\n"
		"last updated 2014-10-03\n", stdout);
}

void
tidy(std::vector<Alarm> &alarms)
{
	AlarmTime now;

	std::sort(alarms.begin(), alarms.end(),
		[](const Alarm &lhs, const Alarm &rhs) {
		return Alarm::cmp(lhs, rhs) < 0;
	});

	auto end = std::unique(alarms.begin(), alarms.end(),
		[](const Alarm &lhs, const Alarm &rhs) {
		return Alarm::cmp(lhs, rhs) == 0;
	});

	alarms.resize(end - alarms.begin());

	now.get_now();

	for (auto alarm = alarms.begin(); alarm != alarms.end(); ) {
		if (AlarmTime::cmp(alarm->time(), now) <= 0) {
			alarm = alarms.erase(alarm);
		}
		else {
			// Because the alarms are in sorted order
			break;
		}
	}
}

int
tidy_main(int argc, char **argv, const char *argstr)
{
	std::vector<Alarm> alarms;
	FILE *f;

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

	tidy(alarms);

	f = fopen(FILE_NAME, "w");
	if (f == NULL) {
		file_err("w");
		return 1;
	}

	for (const Alarm &alarm : alarms) {
		if (alarm.print_file(f) != 0) {
			fclose(f);
			return 1;
		}
	}

	fclose(f);

	return 0;
}