#pragma once

#include <stdio.h>

extern char logbuf[128];

FILE* GetLogFile();
void FlushLogFile();
void LogMessage(char *message);

