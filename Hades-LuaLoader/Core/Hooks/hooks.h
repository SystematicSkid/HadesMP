#pragma once
#include "./directx.h"

namespace core::hooks
{
	std::function<void()> on_update = nullptr;

	PVOID original_update = nullptr;

	void __fastcall hook_update(DWORD64* app, float elapsed_seconds)
	{
		if (on_update)
			on_update();
		return static_cast<void(__fastcall*)(DWORD64*, float)>(original_update)(app, elapsed_seconds);
	}

	void initialize()
	{
		uint8_t shell[] =
		{
			0x48, 0xB8, // mov rax,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0xFF, 0xE0 // jmp rax
		};
		DWORD64 tmp = (DWORD64)&hook_update;
		memcpy(shell + 2, (PVOID)&tmp, sizeof DWORD64);
		size_t hook_length = 12;
		DWORD64 address = engine::addresses::app::functions::update;

		PCHAR trampoline = (PCHAR)malloc(hook_length + 12);
		memcpy(trampoline, (PCHAR)address, hook_length);
		printf("Trampoline: 0x%p\n", trampoline);

		uint8_t shell2[] =
		{
			0x48, 0xB8, // mov rax,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0xFF, 0xE0 // jmp rax
		};

		tmp = (DWORD64)address + hook_length;
		memcpy(shell2 + 2, &tmp, sizeof DWORD64); // put address into shell2
		memcpy(trampoline + hook_length, shell2, sizeof shell2); // insert jmp shell into trampoline
		original_update = trampoline;
		DWORD protection;
		VirtualProtect((PVOID)original_update, hook_length + 12, PAGE_EXECUTE_READWRITE, &protection);

		printf("Original: 0x%p\n", original_update);


		DWORD protect;
		VirtualProtect((PVOID)address, 12, PAGE_EXECUTE_READWRITE, &protect);
		memcpy((PVOID)address, shell, hook_length);
		VirtualProtect((PVOID)address, 12, protect, &protect);

		// Prevent unloading
		address = (uintptr_t)GetModuleHandleA("SDL2.dll") + 0x00ED561;
		printf("Address: 0x%p\n", address);
		uint8_t nop[] = { 0x90,0x90,0x90,0x90,0x90,0x90 };
		VirtualProtect((PVOID)address, 6, PAGE_EXECUTE_READWRITE, &protect);
		memcpy((PVOID)address, nop, 6);
		VirtualProtect((PVOID)address, 6, protect, &protect);
	}
}