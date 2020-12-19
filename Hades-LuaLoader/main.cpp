#include <windows.h>
#include <string>
#include <fstream>
#include "./Core/core.h"

HMODULE dll;
Proxy proxy;
bool init_lua = true;

bool has_loaded_new_player = false;
bool fired = false;

void init()
{
	/*
	DWORD64 stream_ptr;
	if (!engine::PlatformFile::CreateStream(engine::ResourceDirectory::RD_MIDDLEWARE_2, "mod_order.txt", engine::FileMode::FM_READ_BINARY, (engine::FileStream*)&stream_ptr))
		MessageBoxA(NULL, "Error", "Failed to create filestream!", NULL);
	engine::FileStream* stream = (engine::FileStream*)stream_ptr; // TODO: Find a better way to do this T_T
	*/

	FILE* pNewStdout = NULL;
	FILE* pNewStderr = NULL;
	FILE* pNewStdin = NULL;

	/* Initialize our output console */
	// Allocate console
	AllocConsole();
	SetConsoleTitleA("Hades");
	// Assign `stdin`, `stout`, `stderr`
	::freopen_s(&pNewStdin, "CONIN$", "r", stdin);
	::freopen_s(&pNewStdout, "CONOUT$", "w", stdout);
	::freopen_s(&pNewStderr, "CONOUT$", "w", stderr);

	if (!proxy.Init())
		MessageBoxA(NULL, "An error occurred during initialization!", "Fatal Error", MB_ICONASTERISK);


	core::hooks::on_update = [=](engine::App* app,float dt)
	{
		auto player_manager = engine::hades::PlayerManager::Instance();
		auto controllable_unit = player_manager->players[0]->active_unit;
		// We need to verify that the lua handler has already assigned our player's active unit
		if (global::new_map_thing && global::new_unit_data && player_manager && player_manager->players[0]->active_unit && !global::replicated_unit)
		{
			printf("\n\nCreating new unit!\n");
			printf("Location: %f %f\n", global::spawn_location.x, global::spawn_location.y);
			printf("Data: 0x%p\n", global::new_unit_data);
			printf("Thing: 0x%p\n", global::new_map_thing);

			global::replicated_unit = engine::hades::UnitManager::CreatePlayerUnit(global::new_unit_data, global::spawn_location, global::new_map_thing, false, false);
			printf("unit: 0x%p\n", global::replicated_unit);
		}

		if (global::replicated_unit)
		{
			if(player_manager)
			{
				auto player_unit = player_manager->players[0]->active_unit;
				if (player_unit && global::replicated_unit)
				{
					engine::hades::PlayerUnitInput* input = *(engine::hades::PlayerUnitInput**)((uintptr_t)global::replicated_unit + 0x560);
					D3DXVECTOR2 target_location = player_unit->location;
					D3DXVECTOR2 move_location = D3DXVECTOR2();
					if(input)
					{
						//engine::hades::PathFinder::CalcPath(global::replicated_unit, target_location, player_unit, 10.f /*Success Dist*/);
						//input->pPlayerPathfinding->GetNextPathStep(&move_location, target_location);
						global::replicated_unit->MoveInput(&target_location, 1.0, false, dt);
					}
				}
			}
		}

		if (controllable_unit)
		{
			auto unit_type = (engine::hades::Unit*)controllable_unit;

			//printf("Weapon: %s\n", unit_type->arsenal.mControllableWeapons[0]->ToString().c_str());
			/*int len = unit_type->arsenal.mWeapons.size();
			for (int i = 0; i < len; i++)
			{
				printf("[ %iA ] ", i);
				printf("%s\n", unit_type->arsenal.mWeapons[i]->pData->name.ToString());
			}
			len = unit_type->arsenal.mControllableWeapons.size();
			for (int i = 0; i < len; i++)
			{
				printf("[ %iB ] ", i);
				printf("%s\n", unit_type->arsenal.mControllableWeapons[i]->pData->name.ToString());
			}*/
			engine::hades::Weapon* dash = unit_type->arsenal.mControllableWeapons[3];
			if (dash->CanFire())
			{
				// 0 - Dash
				// 1 - Cast?
				// 2/3 - Special
				//printf("Requesting fire...\n");
				//dash->RequestFire(1.f, controllable_unit->location, nullptr);
			}
		}
		// Below is stuff for the original project... loading lua files XD
		/*if(init_lua)
		{
			char* dir = engine::PlatformFile::GetResourceDirectory(engine::ResourceDirectory::RD_MIDDLEWARE_2);
			std::string script_dir(dir);
			std::string order_file = script_dir + "\\mod_order.txt";


			std::streampos fsize = 0;
			std::ifstream file(order_file);

			std::string file_name;
			bool result = true;
			while (std::getline(file, file_name))
			{
				bool result = engine::ScriptManager::LoadFile(file_name.c_str());
				if (!result)
					MessageBoxA(NULL, "Failed to load lua file", "Error", NULL);
			}
			init_lua = false;
		}*/
	};
	core::hooks::initialize();

	//FreeLibraryAndExitThread(dll, 0x1); // We're done, unload
}

bool WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
	if (_Reason != DLL_PROCESS_ATTACH)
		return FALSE;
	dll = (HMODULE)_DllHandle;
	//DisableThreadLibraryCalls((HMODULE)_DllHandle);
	init();
	//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)init, NULL, NULL, NULL);
	return TRUE;
}