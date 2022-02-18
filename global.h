#pragma once

#include <windows.h>

extern CRITICAL_SECTION cs;
extern HANDLE fh;
extern DWORD hostpid;

#define __beginsync EnterCriticalSection(&cs);
#define __endsync LeaveCriticalSection(&cs);