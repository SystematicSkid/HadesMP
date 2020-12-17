#pragma once
#include "./directx.h"
#include <MinHook.h>

/*
	This entire file is pretty ugly simply because
	I did not want to include an external hooking library
*/

namespace core::hooks
{
	std::function<void(float)> on_update = nullptr;

	PVOID original_update = nullptr;

	void __fastcall hook_update(DWORD64* app, float elapsed_seconds)
	{
		if (on_update)
			on_update(elapsed_seconds);
		return static_cast<void(__fastcall*)(DWORD64*, float)>(original_update)(app, elapsed_seconds);
	}

	bool has_loaded_player = false;

	PVOID original_create_player = nullptr;
	engine::hades::Thing* __fastcall hook_create_player_unit(engine::hades::UnitData* unitdata, D3DXVECTOR2 location, engine::hades::MapThing* map_thing, bool do_presentation, bool needs_lua_init)
	{
		if (!has_loaded_player)
		{
			// Allocate new memory space for our copy of UnitData
			global::new_unit_data = (engine::hades::UnitData*)malloc(sizeof engine::hades::UnitData);
			// Copy all existing bytes to our new pointer
			memcpy(global::new_unit_data, unitdata, sizeof engine::hades::UnitData);

			// Fixup all existing pointers by making new copies
			auto new_life_data = (engine::hades::LifeData*)malloc(sizeof engine::hades::LifeData);
			memcpy(new_life_data, unitdata->mLifeData, sizeof engine::hades::LifeData);
			global::new_unit_data->mLifeData = new_life_data;

			auto new_thing_data = (engine::hades::ThingData*)malloc(sizeof engine::hades::ThingData);
			memcpy(new_thing_data, unitdata->mThingData, sizeof engine::hades::ThingData);
			global::new_unit_data->mThingData = new_thing_data;

			auto new_move_data = (engine::hades::MoveAIData*)malloc(sizeof engine::hades::MoveAIData);
			memcpy(new_move_data, unitdata->mMoveAiData, sizeof engine::hades::MoveAIData);
			global::new_unit_data->mMoveAiData = new_move_data;

			auto new_player_data = (engine::hades::PlayerUnitData*)malloc(sizeof engine::hades::PlayerUnitData);
			memcpy(new_player_data, unitdata->mPlayerUnitData, sizeof engine::hades::PlayerUnitData);
			global::new_unit_data->mPlayerUnitData = new_player_data;

			// Increment our new player id
			//global::new_unit_data->mPlayerUnitData->mPlayerIndex++; // This should be set to 2
			//global::new_unit_data->mPlayerControlled = false;

			// Allocate new memory for the new copy of MapThing
			global::new_map_thing = (engine::hades::MapThing*)malloc(sizeof engine::hades::MapThing);
			// Copy all existing data to our new copy
			memcpy(global::new_map_thing, map_thing, sizeof engine::hades::MapThing);

			// I'm not sure if we need to really change anything inside of MapThing... 
			// I will just inc id for now
			global::new_map_thing->mId++;

			// Set the spawn location
			global::spawn_location = D3DXVECTOR2(location);

			has_loaded_player = true;
		}
		else
		{
			// This is a call from our internal module
			// ... do nothing
			// Reset the flag for next call
			has_loaded_player = false;
		}

		auto ret = static_cast<engine::hades::Thing * (__fastcall*)(engine::hades::UnitData*, D3DXVECTOR2, engine::hades::MapThing*, bool, bool)>
			(original_create_player)(unitdata, location, map_thing, do_presentation, needs_lua_init);
		if (!global::original_unit)
			global::original_unit = ret;
		printf("Return: 0x%p\n", ret);

		return ret;
	}

	void hook(DWORD64 address, DWORD64 callback, PVOID* original, int length)
	{
		uint8_t shell[] =
		{
			0x48, 0xB8, // mov rax,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0xFF, 0xE0 // jmp rax
		};
		DWORD64 tmp = (DWORD64)callback;
		memcpy(shell + 2, (PVOID)&tmp, sizeof DWORD64);
		size_t hook_length = length;

		PCHAR trampoline = (PCHAR)malloc(hook_length + sizeof shell);
		memcpy(trampoline, (PCHAR)address, hook_length);

		uint8_t shell2[] =
		{
			0x48, 0xB8, // mov rax,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0xFF, 0xE0 // jmp rax
		};

		tmp = (DWORD64)address + hook_length;
		memcpy(shell2 + 2, &tmp, sizeof DWORD64); // put address into shell2
		memcpy(trampoline + hook_length, shell2, sizeof shell2); // insert jmp shell into trampoline
		*original = trampoline;
		DWORD protection;
		VirtualProtect((PVOID)trampoline, hook_length + sizeof shell, PAGE_EXECUTE_READWRITE, &protection);

		DWORD protect;
		VirtualProtect((PVOID)address, 12, PAGE_EXECUTE_READWRITE, &protect);
		memcpy((PVOID)address, shell, hook_length);
		VirtualProtect((PVOID)address, 12, protect, &protect);
	}

	void initialize()
	{
		// Game App hook 

		hook(engine::addresses::app::functions::update, (DWORD64)&hook_update, &original_update, 12);
		printf("Original: 0x%p\n", original_update);

		MH_Initialize();
		MH_CreateHook((PVOID)engine::addresses::unitmanager::functions::create_player_unit, &hook_create_player_unit, &original_create_player);
		MH_EnableHook((PVOID)engine::addresses::unitmanager::functions::create_player_unit);
		printf("Original: 0x%p\n", original_create_player);
		

		// Script load hook

		// Prevent unloading
		uintptr_t address = (uintptr_t)GetModuleHandleA("SDL2.dll") + 0x00ED561;
		printf("Address: 0x%p\n", address);
		uint8_t nop[] = { 0x90,0x90,0x90,0x90,0x90,0x90 };
		DWORD protect;
		VirtualProtect((PVOID)address, 6, PAGE_EXECUTE_READWRITE, &protect);
		memcpy((PVOID)address, nop, 6);
		VirtualProtect((PVOID)address, 6, protect, &protect);
	}
}