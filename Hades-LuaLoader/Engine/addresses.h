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

		namespace hash
		{
			DWORD64 CONST instance = Memory::GetInstanceAddress("4C 8B 05 ? ? ? ? 4C 03 C3");
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

		namespace unitmanager
		{
			namespace functions
			{
				DWORD64 CONST create_player_unit = Memory::SigScan("48 8B C4 53 57 48 83 EC 58");
			}
		}

		namespace unit
		{
			namespace functions
			{
				DWORD64 CONST move_input = Memory::GetCallAddress("E8 ? ? ? ? 4C 8B 47 18");
			}
		}

		namespace unitstatus
		{
			namespace functions
			{
				DWORD64 CONST can_attack = Memory::SigScan("48 89 5C 24 ? 80 3D ? ? ? ? ? 44 0F B6 DA");
			}
		}

		namespace weapon
		{
			namespace functions
			{
				DWORD64 CONST request_fire = Memory::GetCallAddress("E8 ? ? ? ? 49 83 C4 08");
				DWORD64 CONST tostring = Memory::GetCallAddress("E8 ? ? ? ? 48 8B F0 48 8B 03 4C 8B 80");
				DWORD64 CONST can_fire = Memory::GetCallAddress("E8 ? ? ? ? 84 C0 74 39 8B 4B 30");
			}
		}

		namespace world
		{
			namespace functions
			{
				DWORD64 CONST load_next_map = Memory::GetCallAddress("E8 ? ? ? ? 90 0F B6 45 DF C0 E8 07 24 01 74 10 48 8B 4D C8 48 85 C9 74 07 FF 15 ? ? ? ? 90 0F B6 45 27");
			}
			DWORD64 CONST instance = Memory::GetInstanceAddress("48 89 05 ? ? ? ? 83 78 0C 00 7E 40");
		}

		namespace map
		{
			namespace functions
			{
				DWORD64 CONST load_map = Memory::SigScan("48 8B C4 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 48 C7 45 ? ? ? ? ? 48 89 58 10 48 89 70 18 48 89 78 20 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 37 48 8B D9");
			}
		}
	}
}