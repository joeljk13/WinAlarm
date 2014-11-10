#include "main.h"

#include <string>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void
print_help()
{
	fputs("Usage: alarm COMMAND [OPTIONS]\n\n"

		"COMMAND can be any of the following:\n"
		"\tset - sets an alarm to the time/date given in options\n"
		"\tremove - removes the given alarm(s)\n"
		"\tlist - lists all of the alarms set\n"
		"\ttidy - sorts the alarms and removes expired alarms\n"
		"\tactivate - activates all set alarms\n"
		"\t--help (-h) - displays this help\n"
		"\t--version (-v) - displays the version\n", stdout);
}

static void
print_version()
{
	fputs("alarm - version 1.0\n"
		"last updated 2014-10-03\n", stdout);
}

int
alarm_main(int argc, char **argv)
{
	const char *cmd;
	std::string argstr;
	const std::string space = " ";

	if (argc == 0) {
		print_help();
		return 0;
	}

	cmd = argv[0];

	if (strcmp(cmd, "--help") == 0 || strcmp(cmd, "-h") == 0) {
		print_help();
		return 0;
	}

	if (strcmp(cmd, "--version") == 0 || strcmp(cmd, "-v") == 0) {
		print_version();
		return 0;
	}

	// Restrict argc and argv to just the options now
	--argc;
	++argv;

	argstr = "";
	for (int i = 0; i < argc; ++i) {
		argstr += argv[i] + space;
	}

	srand((unsigned int)time(NULL));

#define CALL_MAIN(s) do { \
	int s ## _main(int, char **, const char *); \
	if (strcmp(cmd, #s) == 0) { \
		return s ## _main(argc, argv, argstr.c_str()); \
	} \
} while (0)

	CALL_MAIN(set);
	CALL_MAIN(remove);
	CALL_MAIN(list);
	CALL_MAIN(tidy);
	CALL_MAIN(activate);

#undef CALL_MAIN

	print_help();

	return 0;
}

int main(int argc, char *argv[])
{
	ASSERT(argc > 0, "Error: you're on a system where argc == 0");

	// Ignore the program name
	int ret = alarm_main(argc - 1, argv + 1);

#ifndef NDEBUG

	if (ret != 0) {
		fprintf(stderr, "\nThere was an error in the application "
			"(error code %i): teminating now.\n", ret);
	}

#endif

	return ret;
}