#include "main.h"
#include "file.h"

#include <string>

static inline int
read_lines(std::vector<std::string> &lines)
{
	FILE *f;

	f = fopen(FILE_NAME, "r");
	if (f == NULL) {
		file_err("r");
		return -1;
	}

	try {
		std::string line;

		line = "";
		for (int c; (c = fgetc(f)) != EOF;) {
			if (c == '\n') {
				lines.push_back(line);
				line = "";
			}
			else {
				line += (char)c;
			}
		}
	}
	catch (...) {
		fclose(f);
		fputs("Error: unable to read file\n", stderr);
		return -1;
	}

	fclose(f);

	return 0;
}

int
read_file(std::vector<Alarm> &alarms)
{
	std::vector<std::string> lines;

	if (read_lines(lines) != 0) {
		return -1;
	}

	for (const std::string &str : lines) {
		Alarm alarm;

		if (alarm.read_file_str(str.c_str()) != 0) {
			return -1;
		}

		try {
			alarms.push_back(alarm);
		}
		catch (...) {
			fputs("Error: unable to allocate memory\n", stderr);
			return -1;
		}
	}

	return 0;
}

void
file_err(const char *mode)
{
	ASSUME(mode != NULL);

	switch (*mode) {
	case 'r':
		fputs("Error: unable to open file '" FILE_NAME "' for reading\n", stderr);
		break;
	case 'a':
	case 'w':
		fputs("Error: unable to open file '" FILE_NAME "' for writing\n", stderr);
		break;
	default:
		ASSUME_UNREACHABLE();
	}
}