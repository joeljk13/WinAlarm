#include "main.h"
#include "time.h"

#include "file.h"

#include <string>

#include <stdio.h>
#include <string.h>
#include <time.h>

void
AlarmTime::add_day()
{
	++day;

	if (day == 32) {
		day = 1;
		++month;

		if (month == 13) {
			month = 1;
			++year;
		}
	}
}

void
AlarmTime::add_minute()
{
	++minute;

	if (minute == 60) {
		minute = 0;
		++hour;
		
		if (hour == 24) {
			hour = 0;
			add_day();
		}
	}
}

int
AlarmTime::print_file(FILE *f) const
{
	ASSUME(f != NULL);

	fprintf(f, "%u-%u-%u %u:%u:%u ", year, month, day,
		hour, minute, second);

	return 0;
}

int
AlarmTime::print_human(FILE *f, int indent) const
{
	std::string tabs;
	const char *tabstr;
	
	ASSUME(f != NULL);

	tabs = "";
	for (int i = 0; i < indent; ++i) {
		tabs += '\t';
	}
	tabstr = tabs.c_str();

	fprintf(f, "%s%u-%u-%u %u:%u:%u \n", tabstr, year, month, day,
		hour, minute, second);

	return 0;
}

int
AlarmTime::cmp(const AlarmTime &lhs, const AlarmTime &rhs)
{
#define CMP(t) if (lhs.t != rhs.t) return lhs.t - rhs.t

	CMP(year);
	CMP(month);
	CMP(day);
	CMP(hour);
	CMP(minute);
	CMP(second);

#undef CMP

	return 0;
}

int
AlarmTime::read_file_str(const char * &str)
{
	if (sscanf(str, "%u-%u-%u %u:%u:%u", &year, &month,
		&day, &hour, &minute, &second)
		!= 6) {
		fputs("Error: invalid file format\n", stderr);
		return -1;
	}

	while (*str++ != ' ') {}
	while (*str++ != ' ') {}

	return 0;
}

int
AlarmTime::read_arg_str(const char *str)
{
	AlarmTime now;

	if (now.get_now() != 0) {
		return -1;
	}

	if (sscanf(str, "%u-%u-%u %u:%u:%u", &year, &month,
		&day, &hour, &minute, &second)
		!= 6) {
		if (sscanf(str, "%u:%u:%u", &hour, &minute,
			&second) != 3) {
			fputs("Error: Invalid time format\n", stderr);
			return -1;
		}

		if (hour > 23 || minute > 59 || second > 59) {
			fputs("Error: Invalid time\n", stderr);
			return -1;
		}

		day = now.day;
		month = now.month;
		year = now.year;
	}

	if (AlarmTime::cmp(*this, now) <= 0) {
		add_day();

		if (AlarmTime::cmp(*this, now) <= 0) {
			fputs("Error: the alarm time must be in the future\n", stderr);
			return -1;
		}
	}

	return 0;
}

int
AlarmTime::get_now()
{
	time_t now_time;
	tm *nowptr;

	if (time(&now_time) == (time_t)-1) {
		return -1;
	}

	nowptr = localtime(&now_time);

	year = nowptr->tm_year + 1900;
	month = nowptr->tm_mon + 1;
	day = nowptr->tm_mday;
	hour = nowptr->tm_hour;
	minute = nowptr->tm_min;
	second = nowptr->tm_sec;

	return 0;
}