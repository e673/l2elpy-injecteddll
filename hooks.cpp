#include "ws2_32.h"
#include <windows.h>
#include "hooks.h"
#include "logger.h"
#include "global.h"
#include "main.h"

// Перехват connect() - занесение сокета в список
int WINAPI User_connect(SOCKET s, struct sockaddr* name, int namelen)
{
	SOCKET target = WS2_32.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	WS2_32.connect(target, name, namelen);

	// Дублируем сокет и отправляем его хосту

	WSAPROTOCOL_INFOW *lpWSAProtocolInfo = (WSAPROTOCOL_INFOW*) malloc(sizeof(WSAPROTOCOL_INFOW));
	WS2_32.WSADuplicateSocket(target, hostpid, lpWSAProtocolInfo);

	// MessageBoxA(0, "Duplicated", "Info", 0);

	sockaddr_in* addr = (sockaddr_in*)name;
	int port = WS2_32.ntohs(addr->sin_port);

	DWORD numwritten;
	WriteFile(fh, &addr->sin_addr.S_un.S_addr, 4, &numwritten, NULL);
	WriteFile(fh, &port, 4, &numwritten, NULL);
	int infosize = sizeof(WSAPROTOCOL_INFOW);
	WriteFile(fh, &infosize, 4, &numwritten, NULL);
	WriteFile(fh, lpWSAProtocolInfo, infosize, &numwritten, NULL);

	Sleep(500);
	WS2_32.closesocket(target);

	free(lpWSAProtocolInfo);

	// Перенаправляем подключение к хосту
	return WS2_32.connect(s, "127.0.0.1", 21903);
}

int WINAPI User_recv(SOCKET s, char* buf, int len, int flags)
{
	__beginsync;
	InterceptedSocket *is = GetInterceptedSocket(s);
	if (is != NULL && !is->Disposed && !is->ThreadStarted)
		StartThreads(is);
	__endsync;

	if (is == NULL)
		return WS2_32.recv(s, buf, len, flags);
	else
	{
		DWORD numread;
		if (!ReadFile(is->clientfile, buf, len, &numread, NULL))
		{
			__beginsync;
			sprintf_s(logbuf, 128, "Error reading client pipe for socket = %d\n", s);
			LogMessage(logbuf);
			__endsync;
			return SOCKET_ERROR;
		}
		else
		{
			__beginsync;
			sprintf_s(logbuf, 128, "recv for client pipe for socket = %d: %d bytes\n", s, numread);
			LogMessage(logbuf);
			__endsync;
			return numread;
		}
	}
}

int WINAPI User_send(SOCKET s, const char* buf, int len, int flags)
{
	__beginsync;
	InterceptedSocket *is = GetInterceptedSocket(s);
	if (is != NULL && !is->Disposed && !is->ThreadStarted)
		StartThreads(is);
	__endsync;

	if (is == NULL)
		return WS2_32.send(s, buf, len, flags);
	else
	{
		DWORD numwrite;
		if (!WriteFile(is->clientfile, buf, len, &numwrite, NULL))
		{
			__beginsync;
			DWORD err = GetLastError();
			sprintf_s(logbuf, 128, "Error writing client pipe for socket = %d, Error = %d\n", s, err);
			LogMessage(logbuf);
			__endsync;
			return SOCKET_ERROR;
		}
		else
		{
			__beginsync;
			sprintf_s(logbuf, 128, "send for client pipe for socket = %d: %d bytes\n", s, numwrite);
			LogMessage(logbuf);
			__endsync;
			return numwrite;
		}
	}
}

int WINAPI User_closesocket(SOCKET s)
{
	__beginsync;
	InterceptedSocket *is = GetInterceptedSocket(s);
	if (is != NULL)
	{
		EndInterception(s);
		sprintf_s(logbuf, 128, "EndInterception(%d)\n", s);
		LogMessage(logbuf);
	}
	__endsync;

	return WS2_32.closesocket(s);
}
