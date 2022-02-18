#pragma once

#include <winsock2.h>
#include <windows.h>

class WS2_32_API
{
private:
	int (WINAPI *Func_WSAStartup)(WORD wVersionRequested, LPWSADATA lpWSAData);

	SOCKET (WINAPI *Func_socket)(int af, int type, int protocol);
	int (WINAPI *Func_connect)(SOCKET s, const struct sockaddr* name, int namelen);
	int (WINAPI *Func_bind)(SOCKET s, const struct sockaddr *name, int namelen);
	int (WINAPI *Func_listen)(SOCKET s, int backlog);
	int (WINAPI *Func_select)(int nfds, fd_set *readfds, fd_set *writefds,  fd_set *exceptfds, const struct timeval *timeout);
	int (WINAPI *Func_recv)(SOCKET s, char* buf, int len, int flags);
	int (WINAPI *Func_send)(SOCKET s, const char* buf, int len, int flags);
	int (WINAPI *Func_closesocket)(SOCKET s);
	int (WINAPI *Func_sendto)(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen);
	SOCKET (WINAPI *Func_accept)(SOCKET s, struct sockaddr *addr, int *addrlen);
	int (WINAPI *Func_shutdown)(SOCKET s, int how);

	unsigned long (WINAPI *Func_inet_addr)(const char* cp);
	u_short (WINAPI *Func_htons)(u_short hostshort);
	u_short (WINAPI *Func_ntohs)(u_short networkshort);

	int (WINAPI *Func_WSAGetLastError)(void);
	void (WINAPI *Func_WSASetLastError)(int iError);

	int (WINAPI *Func_ioctlsocket)(SOCKET s, long cmd, u_long* argp);

	int (WINAPI *Func_WSADuplicateSocket)(SOCKET s, DWORD dwProcessId, LPWSAPROTOCOL_INFO lpProtocolInfo);

public:

	void Init();

	int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);

	SOCKET socket(int af, int type, int protocol);
	SOCKET socket();
	int connect(SOCKET s, const struct sockaddr* name, int namelen);
	int connect(SOCKET s, char *ip, int port);
	int bind(SOCKET s, const struct sockaddr *name, int namelen);
	int listen(SOCKET s, int backlog);
	int select(int nfds, fd_set *readfds, fd_set *writefds,  fd_set *exceptfds, const struct timeval *timeout);
	int recv(SOCKET s, char* buf, int len, int flags);
	int send(SOCKET s, const char* buf, int len, int flags);
	bool sendall(SOCKET s, const char* buf, int len);
	int closesocket(SOCKET s);
	int sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen);
	SOCKET accept(SOCKET s, struct sockaddr *addr, int *addrlen);
	int shutdown(SOCKET s, int how);

	unsigned long inet_addr(const char* cp);
	u_short htons(u_short hostshort);
	u_short ntohs(u_short networkshort);

	int WSAGetLastError(void);
	void WSASetLastError(int iError);

	int ioctlsocket(SOCKET s, long cmd, u_long* argp);

	int WSADuplicateSocket(SOCKET s, DWORD dwProcessId, LPWSAPROTOCOL_INFOW lpProtocolInfo);


	DWORD GetAddr_connect();
	DWORD GetAddr_send();
	DWORD GetAddr_recv();
	DWORD GetAddr_closesocket();
};

extern WS2_32_API WS2_32;