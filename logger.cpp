#include "stdafx.h"
#include "logger.h"

char filename[128];
FILE *log_file = NULL;

char logbuf[128];

FILE* GetLogFile()
{
	if (log_file == NULL)
	{
		sprintf_s(filename, 128, "dllinject.%d.log", GetTickCount());
		fopen_s(&log_file, filename, "w");
	}
	return log_file;
}

void FlushLogFile()
{
	if (log_file != NULL)
		fflush(log_file);
}

void LogMessage(char *message)
{
	fprintf(GetLogFile(), message);
	FlushLogFile();
}
