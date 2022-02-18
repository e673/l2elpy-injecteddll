#include "ws2_32.h"
#include "main.h"
#include <vector>
#include "global.h"
#include "logger.h"

using namespace std;

vector<PInterceptedSocket> InterceptedSockets;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Мосты (связь serverfile с реальным сервером)

#pragma region Bridges

DWORD WINAPI Thread1(LPVOID param)					 
{
	InterceptedSocket *is = (InterceptedSocket*)param;
	char buf[8192];
	DWORD numread;

	__beginsync;
	sprintf_s(logbuf, 128, "Started [Host -> Server] thread for socket = %d\n", is->socket);
	LogMessage(logbuf);
	__endsync;

	// Перегоняем данные, пока это возможно
	while (ReadFile(is->serverfile, buf, 8192, &numread, NULL))
	{
		__beginsync;
		sprintf_s(logbuf, 128, "[Host -> Server] thread for socket = %d: %d bytes received\n", is->socket, numread);
		LogMessage(logbuf);
		__endsync;

		if (!WS2_32.sendall(is->socket, buf, numread))
		{
			int err = WS2_32.WSAGetLastError();
			__beginsync;
			sprintf_s(logbuf, 128, "[Host -> Server] thread for socket = %d: error writing data to server, error = %d\n", is->socket, err);
			LogMessage(logbuf);
			__endsync;
			break;
		}
	}

	int err = GetLastError();
	__beginsync;
	sprintf_s(logbuf, 128, "Ended [Host -> Server] thread for socket = %d, Error = %d\n", is->socket, err);
	LogMessage(logbuf);
	__endsync;

	// Закрываем все соединения по завершении
	__beginsync;
	bool Disposed = is->Disposed;
	is->Disposed = true;
	__endsync;

	if (!Disposed)
	{
		CloseHandle(is->clientfile);
		CloseHandle(is->serverfile);
		WS2_32.shutdown(is->socket, 2);
	}

	return 0;
}

bool WriteFile(HANDLE file, char *buf, int len)
{
	while (len > 0)
	{
		DWORD written;
		if (!WriteFile(file, buf, len, &written, NULL))
			return false;
		buf += written;
		len -= written;
	}
	return true;
}

DWORD WINAPI Thread2(LPVOID param)					 
{
	InterceptedSocket *is = (InterceptedSocket*)param;
	char buf[8192];
	int numread;

	__beginsync;
	sprintf_s(logbuf, 128, "Started [Server - > Host] thread for socket = %d\n", is->socket);
	LogMessage(logbuf);
	__endsync;


	while ((numread = WS2_32.recv(is->socket, buf, 8192, 0)) > 0)
	{
		__beginsync;
		sprintf_s(logbuf, 128, "[Server -> Host] thread for socket = %d: %d bytes received\n", is->socket, numread);
		LogMessage(logbuf);
		__endsync;

		if (!WriteFile(is->serverfile, buf, numread))
		{
			int err = GetLastError();
			__beginsync;
			sprintf_s(logbuf, 128, "[Server -> Host] thread for socket = %d: error writing data to server, error = %d\n", is->socket, err);
			LogMessage(logbuf);
			__endsync;
			break;
		}
	}

	int err = WS2_32.WSAGetLastError();
	__beginsync;
	sprintf_s(logbuf, 128, "Ended [Server - > Host] thread for socket = %d, SocketError = %d\n", is->socket, err);
	LogMessage(logbuf);
	__endsync;

	// Закрываем все соединения по завершении
	__beginsync;
	bool Disposed = is->Disposed;
	is->Disposed = true;
	__endsync;

	if (!Disposed)
	{
		CloseHandle(is->clientfile);
		CloseHandle(is->serverfile);
		WS2_32.shutdown(is->socket, 2);
	}

	return 0;
}

#pragma endregion

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Регистрация сокета

void StartInterception(SOCKET socket, unsigned int addr, short port)
{
	InterceptedSocket *is = GetInterceptedSocket(socket);
	if (is != NULL)
		return;

	// Пытаемся подключиться к хосту =============================================================================
	
	HANDLE firstpipe = CreateFileA("\\\\.\\pipe\\L2ElpyDllInjectClient",
				                  GENERIC_READ | GENERIC_WRITE,
								  NULL,
								  0,
								  OPEN_EXISTING,
								  0,	// non-overlapped
								  NULL);
	if (firstpipe == INVALID_HANDLE_VALUE)
	{
		sprintf_s(logbuf, 128, "Cannot connect to L2Elpy host process (client), socket=%d\n", socket);
		LogMessage(logbuf);
		MessageBoxA(0, "Cannot connect to L2Elpy host process", "DLL inject error", 0);
		return;
	}

	DWORD numwrite;
	DWORD ThisPid = GetCurrentProcessId();
	WriteFile(firstpipe, &ThisPid, 4, &numwrite, NULL);
	WriteFile(firstpipe, &addr, 4, &numwrite, NULL);
	WriteFile(firstpipe, &port, 2, &numwrite, NULL);

	// Создаём второе соединение =================================================================================
	
	HANDLE secondpipe = CreateFileA("\\\\.\\pipe\\L2ElpyDllInjectServer",
				                  GENERIC_READ | GENERIC_WRITE,
								  NULL,
								  0,
								  OPEN_EXISTING,
								  0,	// non-overlapped
								  NULL);
	if (secondpipe == INVALID_HANDLE_VALUE)
	{
		CloseHandle(firstpipe);
		sprintf_s(logbuf, 128, "Cannot connect to L2Elpy host process (server), socket=%d\n", socket);
		LogMessage(logbuf);
		MessageBoxA(0, "Cannot connect to L2Elpy host process", "DLL inject error", 0);
		return;
	}

	WriteFile(secondpipe, &ThisPid, 4, &numwrite, NULL);

	// Всё готово для регистрации соединения =====================================================================

	is = new InterceptedSocket();
	is->socket = socket;
	is->clientfile = firstpipe;
	is->serverfile = secondpipe;
	is->ThreadStarted = false;
	is->Disposed = false;
	InterceptedSockets.push_back(is);
}

// Запуск потоков
void StartThreads(PInterceptedSocket is)
{
	if (!is->ThreadStarted)
	{
		is->ThreadStarted = true;
		is->Thread1 = CreateThread(NULL, 65536, Thread1, is, 0, 0);
		is->Thread2 = CreateThread(NULL, 65536, Thread2, is, 0, 0);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Получение статуса перехвата

PInterceptedSocket GetInterceptedSocket(SOCKET socket)
{
	int count = InterceptedSockets.size();
	for (int i = 0; i < count; i++)
	{
		InterceptedSocket *is = InterceptedSockets[i];
		if (is->socket == socket)
			return is;
	}

	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Завершение перехвата

bool EndInterception(SOCKET socket)
{
	int count = InterceptedSockets.size();
	for (int i = 0; i < count; i++)
	{
		InterceptedSocket *is = InterceptedSockets[i];
		if (is->socket == socket)
		{
			if (!is->Disposed)
			{
				CloseHandle(is->clientfile);
				CloseHandle(is->serverfile);
				WS2_32.shutdown(is->socket, 2);
			}
			is->Disposed = true;
			InterceptedSockets.erase(InterceptedSockets.begin() + i);
			return true;
		}
	}

	return false;
}
