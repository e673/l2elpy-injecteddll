#include "ws2_32.h"

WS2_32_API WS2_32;
DWORD Global_Entry_Connect;

void WS2_32_API::Init()
{
	HMODULE ws2_32 = GetModuleHandleA("ws2_32.dll");
	if (ws2_32 == 0)
		ws2_32 = LoadLibraryA("ws2_32.dll");

	(FARPROC&)Func_WSAStartup = GetProcAddress(ws2_32, "WSAStartup");

	(FARPROC&)Func_socket = GetProcAddress(ws2_32, "socket");
	(FARPROC&)Func_connect = GetProcAddress(ws2_32, "connect");
	(FARPROC&)Func_bind = GetProcAddress(ws2_32, "bind");
	(FARPROC&)Func_listen = GetProcAddress(ws2_32, "listen");
	(FARPROC&)Func_accept = GetProcAddress(ws2_32, "accept");
	(FARPROC&)Func_select = GetProcAddress(ws2_32, "select");
	(FARPROC&)Func_closesocket = GetProcAddress(ws2_32, "closesocket");
	(FARPROC&)Func_inet_addr = GetProcAddress(ws2_32, "inet_addr");
	(FARPROC&)Func_send = GetProcAddress(ws2_32, "send");
	(FARPROC&)Func_recv = GetProcAddress(ws2_32, "recv");
	(FARPROC&)Func_sendto = GetProcAddress(ws2_32, "sendto");
	(FARPROC&)Func_shutdown = GetProcAddress(ws2_32, "shutdown");

	(FARPROC&)Func_htons = GetProcAddress(ws2_32, "htons");
	(FARPROC&)Func_ntohs = GetProcAddress(ws2_32, "ntohs");

	(FARPROC&)Func_WSAGetLastError = GetProcAddress(ws2_32, "WSAGetLastError");
	(FARPROC&)Func_WSASetLastError = GetProcAddress(ws2_32, "WSASetLastError");

	(FARPROC&)Func_ioctlsocket = GetProcAddress(ws2_32, "ioctlsocket");
	(FARPROC&)Func_WSADuplicateSocket = GetProcAddress(ws2_32, "WSADuplicateSocketW");

	Global_Entry_Connect = (DWORD)Func_connect + 5;
}

// ============================================================================================================

_declspec(naked) int WINAPI Original_connect(SOCKET s, const sockaddr *name, int namelen)
{
	__asm 
	{ 
		mov edi,edi 
			push ebp 
			mov ebp,esp 
			jmp [Global_Entry_Connect]
	}
}

int WS2_32_API::WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData)
{
	return Func_WSAStartup(wVersionRequested, lpWSAData);
}

SOCKET WS2_32_API::socket(int af, int type, int protocol)
{
	return Func_socket(af, type, protocol);
}

SOCKET WS2_32_API::socket()
{
	return Func_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

int WS2_32_API::connect(SOCKET s, const sockaddr *name, int namelen)
{
	return Original_connect(s, name, namelen);
}

int WS2_32_API::connect(SOCKET s, char *ip, int port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = Func_inet_addr(ip);
	addr.sin_port = Func_htons(port);
	return connect(s, (sockaddr*)&addr, sizeof(addr));
}

int WS2_32_API::bind(SOCKET s, const sockaddr *name, int namelen)
{
	return Func_bind(s, name, namelen);
}

int WS2_32_API::listen(SOCKET s, int backlog)
{
	return Func_listen(s, backlog);
}

int WS2_32_API::select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const timeval *timeout)
{
	return Func_select(nfds, readfds, writefds, exceptfds, timeout);
}

int WS2_32_API::recv(SOCKET s, char *buf, int len, int flags)
{
	return Func_recv(s, buf, len, flags);
}

int WS2_32_API::send(SOCKET s, const char *buf, int len, int flags)
{
	return Func_send(s, buf, len, flags);
}

bool WS2_32_API::sendall(SOCKET s, const char* buf, int len)
{
	while (len > 0)
	{
		int res = send(s, buf, len, 0);
		if (res <= 0)
			return false;

		buf += res;
		len -= res;
	}

	return true;
}

int WS2_32_API::closesocket(SOCKET s)
{
	return Func_closesocket(s);
}

int WS2_32_API::sendto(SOCKET s, const char *buf, int len, int flags, const sockaddr *to, int tolen)
{
	return Func_sendto(s, buf, len, flags, to, tolen);
}

SOCKET WS2_32_API::accept(SOCKET s, sockaddr *addr, int *addrlen)
{
	return Func_accept(s, addr, addrlen);
}

int WS2_32_API::shutdown(SOCKET s, int how)
{
	return Func_shutdown(s, how);
}

unsigned long WS2_32_API::inet_addr(const char* cp)
{
	return Func_inet_addr(cp);
}

u_short WS2_32_API::htons(u_short hostshort)
{
	return Func_htons(hostshort);
}

u_short WS2_32_API::ntohs(u_short networkshort)
{
	return Func_ntohs(networkshort);
}

int WS2_32_API::WSAGetLastError(void)
{
	return Func_WSAGetLastError();
}

void WS2_32_API::WSASetLastError(int iError)
{
	Func_WSASetLastError(iError);
}

int WS2_32_API::ioctlsocket(SOCKET s, long cmd, u_long* argp)
{
	return Func_ioctlsocket(s, cmd, argp);
}

int WS2_32_API::WSADuplicateSocket(SOCKET s, DWORD dwProcessId, LPWSAPROTOCOL_INFOW lpProtocolInfo)
{
	return Func_WSADuplicateSocket(s, dwProcessId, lpProtocolInfo);
}

// ============================================================================================================

DWORD WS2_32_API::GetAddr_connect()
{
	return (DWORD)Func_connect;
}

DWORD WS2_32_API::GetAddr_send()
{
	return (DWORD)Func_send;
}

DWORD WS2_32_API::GetAddr_recv()
{
	return (DWORD)Func_recv;
}

DWORD WS2_32_API::GetAddr_closesocket()
{
	return (DWORD)Func_closesocket;
}
