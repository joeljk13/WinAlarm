#include "main.h"

#include "alarm.h"
#include "file.h"

#include <stdio.h>
#include <string.h>

static void
print_help()
{
	fputs("Usage: alarm set [DATE] [TIME] [OPTIONS]\n\n"

		"Sets an alarm at the given time.\n\n"

		"If --help or --version (or their short versions) are used as\n"
		"options, only the first one will be used, and no time or date\n"
		"should be specified. Otherwise, TIME is mandatory.\n\n"

		"DATE uses the format is YYYY-MM-DD. If the date is omitted,\n"
		"today's date is used if the time has not already passed;\n"
		"otherwise tomorrow's date is used.\n\n"

		"TIME is mandatory if neither --help or --version is used.\n"
		"The format for time is HH:MM:SS, using a 24-hour system.\n\n"

		"If multiple alarm levels are given, most severe one is used.\n\n"

		"OPTIONS can be any of the following, in (almost) any order:\n"
		"\t--help (-h) - display this help. If specified, this must be the first\n"
		"\t\toption, and neither TIME nor DATE can be specified.\n"
		"\t--version (-v) - display the version. If specified, this must be the\n"
		"\t\tfirst option, and neither TIME nor DATE can be specified.\n"
		"\t--critical (-c) - the alarm is of critical level\n"
		"\t--warning (-w) - the alarm is of warning level\n"
		"\t--nosleep (-ns) - prevent the user from sleeping this alarm\n"
		"\t--nosound (-nd) - prevent the alarm from making sound\n"
		"\t--novisual (-nv) - prevent the alarm from making visual effects.\n"
		"\t--nowake (-nk) - only raise an alarm if the computer is already awake.\n",
			stdout);
}

static void
print_version()
{
	fputs("alarm set - version 1.0\n"
		"last updated 2014-10-03\n", stdout);
}

int
set_main(int argc, char **argv, const char *argstr)
{
	Alarm alarm;
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

	if (alarm.read_arg_str(argstr) != 0) {
		print_help();
		return 1;
	}
	
	f = fopen(FILE_NAME, "a");
	if (f == NULL) {
		file_err("a");
		return 1;
	}

	if (alarm.print_file(f) != 0) {
		fclose(f);
		return 1;
	}

	fclose(f);

	return 0;
}