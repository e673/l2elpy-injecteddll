#pragma once

// ============================================================================
//
//   INJECTOR - установка jmp-хуков
//
//   Initialization:
//     have to call InitInjector
//
// ============================================================================

#include <windows.h>

class CInjector
{
public:
	void Init();
	int Set_Hook(DWORD func_ptr, DWORD hook_ptr);
};

extern CInjector Injector;

/////////////////////////////////////////////////////////////////////////////
// 
// Функция Set_Hook ставит JMP-хук в функцию
//
// old_ptr - адрес старой функции
// new_ptr - адрес новой функции
//
// Коды ошибок:
// 0 = no error
// 1 = VirtualProtect Error
// 2 = Write Error (exception)
// 3 = Write Error (number of bytes != 5)
// 4 = Not initialized

