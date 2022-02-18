#pragma once

#include <windows.h>

// Структура - перехваченный сокет
typedef struct InterceptedSocket
{
	// Первичный ключ
	SOCKET socket;

	// Пайп со стороны клиента
	HANDLE clientfile;

	// Пайп со стороны сервера
	HANDLE serverfile;

	// Потоки для создания моста
	HANDLE Thread1, Thread2;

	// Отметка о запуске нитей
	bool ThreadStarted;

	// Отметка об очистке
	bool Disposed;

} InterceptedSocket;

typedef InterceptedSocket* PInterceptedSocket;

// Получени структуры
PInterceptedSocket GetInterceptedSocket(SOCKET socket);

// Регистрация сокета
void StartInterception(SOCKET socket, unsigned int addr, short port);

// Запуск потоков
void StartThreads(PInterceptedSocket is);

// Завершение перехвата
bool EndInterception(SOCKET socket);
