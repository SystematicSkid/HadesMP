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

		namespace app
		{
			namespace functions
			{
				DWORD64 CONST update = Memory::GetCallAddress("E8 ? ? ? ? 80 7B 52 00");
			}
		}

		namespace renderer
		{
			DWORD64 CONST instance = Memory::GetInstanceAddress("48 8B 0D ? ? ? ? 48 85 C9 75 07");
		}

		namespace debug_overlay
		{
			namespace functions
			{
				DWORD64 CONST addtext = Memory::GetCallAddress("E8 ? ? ? ? 8B 45 77");
			}
		}

		namespace camera
		{
			namespace functions
			{
				DWORD64 CONST instance = Memory::GetCallAddress("E8 ? ? ? ? 80 78 03 00");
				DWORD64 CONST shake = Memory::GetCallAddress("E8 ? ? ? ? C7 45 ? ? ? ? ? 48 8D 55 D7");
			}
		}

		namespace playermanager
		{
			namespace functions
			{
				DWORD64 CONST instance = Memory::GetCallAddress("E8 ? ? ? ? 0F B6 4D 04");
				DWORD64 CONST add_player = Memory::GetCallAddress("E8 ? ? ? ? 48 8B F8 0F B6 1D");
			}
		}
	}
}