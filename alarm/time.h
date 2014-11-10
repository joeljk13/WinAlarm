#ifndef TIME_H_
#define TIME_H_ 1

#include "main.h"

#include <stdio.h>

struct AlarmTime {

	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;

	void add_day();
	void add_minute();

	int print_file(FILE *f) const;

	int print_human(FILE *f, int indent = 0) const;

	/* On success, moves str to after where the time is specified. */
	int read_file_str(const char *&str);

	int read_arg_str(const char *str);

	int get_now();

	/* Retuns 0 if lhs is the same time as rhs, <0 if lhs comes before rhs, and
	>0 if lhs comes after rhs. */
	static int cmp(const AlarmTime &lhs, const AlarmTime &rhs);

};

#endif