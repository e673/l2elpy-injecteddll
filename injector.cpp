#include "injector.h"
#include <windows.h>
#include <stdio.h>

// GGbypass

DWORD Entry_WriteProcessMemory = 0;
DWORD Entry_VirtualProtect = 0;
DWORD Entry_connect = 0;

_declspec(naked) BOOL WINAPI Original_WriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesWritten)
{ 
     __asm 
    { 
          mov edi,edi 
          push ebp 
          mov ebp,esp 
          jmp [Entry_WriteProcessMemory]
    }
}

_declspec(naked) BOOL WINAPI Original_VirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{ 
     __asm 
    { 
          mov edi,edi 
          push ebp 
          mov ebp,esp 
          jmp [Entry_VirtualProtect]
    }
}

#pragma pack(1)

// Структура - команда условного перехода
struct jmp_far
{
	BYTE instr_jmp;    //здесь будет код инструкции jmp
	DWORD arg;         //аргумент push
};

// ============================================================================================================

void CInjector::Init()
{
	Entry_VirtualProtect = (DWORD)VirtualProtect + 5;
	Entry_WriteProcessMemory = (DWORD)WriteProcessMemory + 5;
}

int CInjector::Set_Hook(DWORD func_ptr, DWORD hook_ptr)
{
	if (Entry_WriteProcessMemory == 0)
		return 4;

	jmp_far jump;
	jump.instr_jmp = 0xE9;
	jump.arg = hook_ptr - func_ptr - 5;

	DWORD op, written;

	// Обычно страницы в этой области недоступны для записи
	// поэтому принудительно разрешаем запись
	BOOL vp_res = Original_VirtualProtect((LPVOID)func_ptr, 5, PAGE_READWRITE, &op);
	if (!vp_res)
	{
		char msg[128];
		sprintf_s(msg, 128, "VirtualProtect at %d call fail, old_op = %d, code = %d", func_ptr, op, GetLastError());
	    MessageBoxA(NULL, msg, "Error!", 0);
	    return 1;
	}
  
	// Пишем новый адрес
	// WriteProcessMemory(hprocess, (LPVOID)old_ptr, (void*)(&jump), 5, &written);
        // GG перехватывает этот вызов, поэтому пишем прям так

	try
	{
		memcpy((void*)func_ptr, (void*)&jump, 5);
		written = 5;
	}
	catch (...)
	{
		written = 0;
		return 2;
	}

	// Восстанавливаем первоначальную защиту области по записи
	Original_VirtualProtect((LPVOID)func_ptr, 5, op, &op);

	// Если записать не удалось - увы, все пошло прахом:
	if (written != 5)
	{
		char msg[128];
		sprintf_s(msg, 128, "Unable rewrite address in InterceptCall(%d, %d)", func_ptr, hook_ptr);
	    MessageBoxA(NULL, msg, "Error!", 0);
	    return 3;
	}

	return 0;
}

CInjector Injector;



