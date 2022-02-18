#include "ws2_32.h"
#include <windows.h>
#include "injector.h"
#include <stdio.h>
#include "hooks.h"
#include "logger.h"
#include "pubdata.h"
#include "global.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
				// Инициализация локера
				InitializeCriticalSection(&cs);

				// Инициализация API
				WS2_32.Init();

				// Перехват функции connect()
				Injector.Init();

				DWORD addr_connect = WS2_32.GetAddr_connect();
				PublicData.MyConnect = (DWORD)User_connect;
				PublicData.OriginalConnect = addr_connect;

				DWORD addr_send = WS2_32.GetAddr_send();
				PublicData.MySend = (DWORD)User_send;
				PublicData.OriginalSend = addr_send;

				DWORD addr_recv = WS2_32.GetAddr_recv();
				PublicData.MyRecv = (DWORD)User_recv;
				PublicData.OriginalRecv = addr_recv;

				DWORD addr_closesocket = WS2_32.GetAddr_closesocket();
				PublicData.MyClose = (DWORD)User_closesocket;
				PublicData.OriginalClose = addr_closesocket;

				int code = Injector.Set_Hook(addr_connect, (DWORD)User_connect);

				/*char buf[128];
				sprintf_s(buf, 128, "connect() entry: %d", addr_connect);
				MessageBoxA(0, buf, "Info", 0);*/

				if (*(char*)addr_connect != -23)
				{
					char buf[128];
					sprintf_s(buf, 128, "L2Elpy error: connect() was not intercepted, code = %d", code);
					MessageBoxA(0, buf, "Error", 0);
				}

				// Подключение к хосту
				fh = CreateFileA("\\\\.\\pipe\\strans", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
				if (fh == INVALID_HANDLE_VALUE)
					MessageBoxA(0, "Cannot connect to the pipe", "Error", 0);

				DWORD numread;
				ReadFile(fh, &hostpid, 4, &numread, NULL);
				if (numread != 4)
					MessageBoxA(0, "Pipe read error", "Error", 0);

			}
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			break;
	}
	
	return TRUE;
}
