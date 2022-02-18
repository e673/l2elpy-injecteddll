#pragma once

#include <windows.h>

int WINAPI User_connect(SOCKET s, struct sockaddr* name, int namelen);
int WINAPI User_recv(SOCKET s, char* buf, int len, int flags);
int WINAPI User_send(SOCKET s, const char* buf, int len, int flags);
int WINAPI User_closesocket(SOCKET s);
