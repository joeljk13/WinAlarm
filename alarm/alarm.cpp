#include "main.h"
#include "alarm.h"

#define ALARM_SLEEP_MINUTES 8

Alarm::Alarm(const AlarmTime &alarm_time, const AlarmOptions &alarm_options) :
	time_(alarm_time),
	options_(alarm_options)
{}

int
Alarm::print_file(FILE *f) const
{
	if (time_.print_file(f) != 0) {
		return -1;
	}

	if (options_.print_file(f) != 0) {
		return -1;
	}

	fputc('\n', f);

	return 0;
}

int
Alarm::print_human(FILE *f, int indent) const
{
	if (time_.print_human(f, indent) != 0) {
		return -1;
	}

	if (options_.print_human(f, indent + 1) != 0) {
		return -1;
	}

	return 0;
}

int
Alarm::read_file_str(const char *str)
{
	if (time_.read_file_str(str) != 0) {
		return -1;
	}

	if (options_.read_file_str(str) != 0) {
		return -1;
	}

	return 0;
}

int
Alarm::read_arg_str(const char *str)
{
	if (time_.read_arg_str(str) != 0) {
		return -1;
	}

	if (options_.read_arg_str(str) != 0) {
		return -1;
	}

	return 0;
}

void
Alarm::sleep()
{
	for (int i = 0; i < ALARM_SLEEP_MINUTES; ++i) {
		time_.add_minute();
	}
}

const AlarmTime &
Alarm::time() const
{
	return time_;
}

const AlarmOptions &
Alarm::options() const
{
	return options_;
}

int
Alarm::cmp(const Alarm &lhs, const Alarm &rhs)
{
	return AlarmTime::cmp(lhs.time_, rhs.time_);
}