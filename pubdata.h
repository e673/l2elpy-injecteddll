#pragma once

#include <windows.h>

typedef struct PubData
{
	DWORD MyConnect;
	DWORD OriginalConnect;

	DWORD MySend;
	DWORD OriginalSend;

	DWORD MyRecv;
	DWORD OriginalRecv;

	DWORD MyClose;
	DWORD OriginalClose;

} PubData;

extern "C" __declspec (dllexport) PubData PublicData;