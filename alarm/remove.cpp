#include "main.h"

#include "alarm.h"
#include "file.h"
#include "time.h"

#include <vector>

#include <stdio.h>
#include <string.h>

static void
print_help()
{
	fputs("Usage: alarm remove [DATE] TIME\n\n"
		"Removes the given alarm(s).\n\n"
		"If DATE is not specified, today's date is used, unless TIME has\n"
		"already passed, in which case tomorrow's date is used.\n\n"
		"If DATE is not specified and TIME is 'all',\n"
		"then all alarms are removed.\n", stdout);
}

static void
print_version()
{
	fputs("alarm remove - version 1.0\n"
		"last updated 2014-10-03\n", stdout);
}

int
remove_main(int argc, char **argv, const char *argstr)
{
	std::vector<Alarm> alarms;
	AlarmTime alarm_time;
	FILE *f;
	
	if (argc == 0) {
		print_help();
		return 0;
	}
	
	if (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "-h") == 0) {
		print_help();
		return 0;
	}
	
	if (strcmp(argv[0], "--version") == 0 || strcmp(argv[0], "-v") == 0) {
		print_version();
		return 0;
	}
	
	if (strcmp(argv[0], "all") == 0) {
		f = fopen(FILE_NAME, "w");
		if (f == NULL) {
			file_err("w");
			return 1;
		}

		fclose(f);

		return 0;
	}

	if (alarm_time.read_arg_str(argstr) != 0) {
		return 1;
	}

	if (read_file(alarms) != 0) {
		return 1;
	}

	for (auto alarm = alarms.begin(); alarm != alarms.end(); ++alarm) {
		if (AlarmTime::cmp(alarm->time(), alarm_time) == 0) {
			alarms.erase(alarm);
			break;
		}
	}

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