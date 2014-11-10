#include "main.h"
#include "options.h"

#include <string>

int
AlarmOptions::print_file(FILE *f) const
{
	ASSUME(f != NULL);

	fprintf(f, "%u%u%u%u%u ", (unsigned int)level, (unsigned int)sleep,
		(unsigned int)sound, (unsigned int)visual, (unsigned int)wake);

	return 0;
}

static inline const char *
booltostr(bool b) {
	return b ? "yes" : "no";
}

int
AlarmOptions::print_human(FILE *f, int indent) const
{
	std::string tabs;
	const char *tabstr;
	
	tabs = "";
	for (int i = 0; i < indent; ++i) {
		tabs += '\t';
	}
	tabstr = tabs.c_str();

	fprintf(f, "%sAllow sleep: %s\n%sLevel: %s\n%sUse sound: %s\n"
		"%sUse visual: %s\n%sWake computer if asleep: %s\n",
		tabstr, booltostr(sleep), tabstr,
		level == AlarmLevel::CRITICAL ?
			"critical" :
			level == AlarmLevel::WARNING ?
				"warning" :
				"normal", tabstr, booltostr(sound), tabstr, booltostr(visual),
		tabstr, booltostr(wake));

	return 0;
}

int
AlarmOptions::read_arg_str(const char *str)
{
#define ARG(n,a) n = strstr(str, "--no" #n) == NULL && strstr(str, a) == NULL

	ARG(sleep, "-ns");
	ARG(sound, "-nd");
	ARG(visual, "-nv");
	ARG(wake, "-nw");

#undef ARG

	level = strstr(str, "--critical") != NULL || strstr(str, "-c") != NULL ?
		AlarmLevel::CRITICAL :
		strstr(str, "--warning") != NULL || strstr(str, "-w") != NULL ?
			AlarmLevel::WARNING :
			AlarmLevel::NORMAL;

	return 0;
}

int
AlarmOptions::read_file_str(const char *&str)
{
	unsigned int level, sleep, sound, visual, wake;

	if (sscanf(str, "%1u%1u%1u%1u%1u", &level, &sleep, &sound, &visual, &wake) != 5) {
		return -1;
	}

	switch (sleep | sound | visual | wake) {
	case 0:
	case 1:
		break;
	default:
		fputs("Error: invalid file format\n", stderr);
		return -1;
	}

	switch (level) {
	case AlarmLevel::CRITICAL:
	case AlarmLevel::WARNING:
	case AlarmLevel::NORMAL:
		break;
	default:
		fputs("Error: invalid file format\n", stderr);
		return -1;
	}

	this->sleep = !!sleep;
	this->sound = !!sound;
	this->visual = !!visual;
	this->wake = !!wake;
	this->level = (AlarmLevel)level;

	while (*str++ != ' ') {}

	return 0;
}