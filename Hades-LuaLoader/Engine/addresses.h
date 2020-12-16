#pragma once
#include <windows.h>

namespace engine
{
	DWORD_PTR module_engine = reinterpret_cast<DWORD_PTR>(GetModuleHandleA("EngineWin64s.dll"));

	namespace addresses
	{
		namespace lua
		{
			namespace functions
			{
				DWORD64 CONST loadfile = Memory::GetCallAddress("E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 4C 8D 05 ? ? ? ? 48 8D 15");
			}
			DWORD64 CONST state = Memory::GetInstanceAddress("48 8B 0D ? ? ? ? 48 85 C9 75 03");
		}

		namespace platformfile
		{
			namespace functions
			{
				DWORD64 CONST createstream = Memory::GetCallAddress("E8 ? ? ? ? 4C 8B 65 B8");
				DWORD64 CONST get_resource_directory = Memory::GetCallAddress("E8 ? ? ? ? 4C 8D 44 24 ? 48 8B C8");
			}

			DWORD64 CONST directories = Memory::GetInstanceAddress("48 8D 05 ? ? ? ? 49 8B 04 02");
		}
	}
}