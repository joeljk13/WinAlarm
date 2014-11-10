#ifndef OPTIONS_H_
#define OPTIONS_H_ 1

#include "main.h"

#include <stdio.h>

enum AlarmLevel : unsigned int {
	NORMAL,
	WARNING,
	CRITICAL
};

struct AlarmOptions {

	AlarmLevel level;
	bool sleep;
	bool sound;
	bool visual;
	bool wake;

	int print_file(FILE *f) const;

	int print_human(FILE *f, int indent = 0) const;

	int read_arg_str(const char *str);

	/* On success, moves str to after where the options are specified. */
	int read_file_str(const char *&str);

};

#endif