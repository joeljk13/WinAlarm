#ifndef FILE_H_
#define FILE_H_ 1

#include "main.h"

#include "alarm.h"

#include <vector>

#define FILE_NAME "alarmfile.txt"

int
read_file(std::vector<Alarm> &alarms);

void
file_err(const char *mode);

#endif