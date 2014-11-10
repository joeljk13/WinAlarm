#include "main.h"

#include "alarm.h"
#include "file.h"

#include <vector>

#include <stdio.h>
#include <string.h>

static void
print_help()
{
	fputs("Usage: alarm list\n\n"
		"Lists all the set alarms.\n", stdout);
}

static void
print_version()
{
	fputs("alarm list - version 1.0\n"
		"last updated 2014-10-03\n", stdout);
}

int
list_main(int argc, char **argv, const char *argstr)
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

	for (const Alarm &alarm : alarms) {
		if (alarm.print_human(stdout) != 0) {
			return 1;
		}
	}
	
	return 0;
}