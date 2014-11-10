#ifndef ALARM_H_
#define ALARM_H_ 1

#include "main.h"

#include "time.h"
#include "options.h"

#include <stdio.h>

class Alarm {

public:

	Alarm() = default;

	Alarm(const AlarmTime &alarm_time, const AlarmOptions &alarm_options);

	int print_file(FILE *f) const;

	int print_human(FILE *f, int indent = 0) const;

	int read_file_str(const char *str);

	int read_arg_str(const char *str);

	void sleep();

	const AlarmTime &time() const;
	const AlarmOptions &options() const;

	static int cmp(const Alarm &lhs, const Alarm &rhs);

private:

	AlarmTime time_;
	AlarmOptions options_;

};

#endif