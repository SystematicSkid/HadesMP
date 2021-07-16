#pragma once
#include "./directx.h"
#include <minhook/include/MinHook.h>
#include <any>

namespace core::hooks
{
	std::function<void(engine::App*, float)> on_update = nullptr;
	std::function<void(PVOID, float)> on_draw = nullptr;


	PVOID original_update = nullptr;

	void __fastcall hook_update(engine::App* app, float elapsed_seconds)
	{
		if (on_update)
			on_update(app, elapsed_seconds);
		return static_cast<void(__fastcall*)(engine::App*, float)>(original_update)(app, elapsed_seconds);
	}

	bool has_loaded_player = false;

	PVOID original_create_player = nullptr;
	engine::hades::Thing* __fastcall hook_create_player_unit(engine::hades::UnitData* unitdata, D3DXVECTOR2 location, engine::hades::MapThing* map_thing, bool do_presentation, bool needs_lua_init)
	{

		if (!has_loaded_player)
		{
			/* This is our localplayer loading in */
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
			// Reset the flag for next call -- Note: This is not required if we hook `load_next_map`
			//has_loaded_player = false;
		}

		auto ret = static_cast<engine::hades::Thing * (__fastcall*)(engine::hades::UnitData*, D3DXVECTOR2, engine::hades::MapThing*, bool, bool)>
			(original_create_player)(unitdata, location, map_thing, do_presentation, needs_lua_init);
		if (!global::original_unit)
		{

			global::original_unit = ret;
		}

		return ret;
	}

	PVOID original_request_fire = nullptr;
	bool __fastcall hook_request_fire(engine::hades::Weapon* weapon, float angle, D3DXVECTOR2 target_location, engine::hades::Thing* target, bool apply_self_effects, D3DXVECTOR2 from_location)
	{
		auto player_manager = engine::hades::PlayerManager::Instance();
		auto controllable_unit = (engine::hades::Unit*)player_manager->players[0]->active_unit;
		
		//printf("Weapon: %s\nAngle: %f\nLocation: %f %f\nTarget: 0x%p\n", weapon->pData->name.ToString(), angle, target_location.x, target_location.y, target);
		
		//int weapon_id = weapon->pData->name.id;
		//printf("Weapon %s\n", weapon->pData->name.ToString());
		/* Localplayer attack */
		if(controllable_unit)
		{
			if (weapon->mOwnerId == controllable_unit->mId)
			{
				/* Tell replicated unit to attack */
				//engine::hades::Weapon* repl_weapon = global::replicated_unit->arsenal.GetWeapon(weapon->pData->name);
				//if(repl_weapon)
				//	repl_weapon->RequestFire(global::replicated_unit->GetAngle(target_location), target_location, target);
				if (weapon->IsReadyToFire() && controllable_unit->IsReadyToFire())
				{
					msgpack::zone z;
					/* Send attack packet */
					network::packets::AttackPacket attack_packet(weapon, angle, target_location, target, apply_self_effects, from_location);
					network::packets::Packet packet(network::packets::PacketType::OnAttack, msgpack::object(attack_packet, z));
					std::stringstream ss;
					msgpack::pack(ss, packet);
					network::client.Send(ss.str());
				}
			}
		}

		return reinterpret_cast<bool(__fastcall*)(engine::hades::Weapon*, float, D3DXVECTOR2, engine::hades::Thing*, bool, D3DXVECTOR2)>(original_request_fire)
			(weapon, angle, target_location, target, apply_self_effects, from_location);
	}

	PVOID original_load_next_map = nullptr;
	void __fastcall hook_load_next_map(engine::hades::World* world, std::string* map_name, int request, engine::misc::Color loading_bg_color)
	{
		has_loaded_player = false;
		/* Reset our unit pointers */
		global::original_unit = nullptr;
		global::replicated_unit = nullptr;
		//printf("Loading map: %s\n", map_name->c_str());
		return static_cast<void(__fastcall*)(engine::hades::World*, std::string*, int, engine::misc::Color)>(original_load_next_map)(world, map_name, request, loading_bg_color);
	}

	PVOID original_draw = nullptr;
	void __fastcall hook_gameplayscreen_draw(PVOID thisptr, float elapsed_seconds)
	{
		if (on_draw)
			on_draw(thisptr, elapsed_seconds);
		return static_cast<void(__fastcall*)(PVOID, float)>(original_draw)(thisptr, elapsed_seconds);
	}

	PVOID original_check_team = nullptr;
	bool __fastcall hook_has_same_team(engine::hades::Thing* thisptr, engine::hades::Thing* other)
	{
		if (global::original_unit && global::replicated_unit)
		{
			// Check if they are the same
			if (thisptr == global::original_unit && other == global::replicated_unit)
				return true;

			if (thisptr == global::replicated_unit && other == global::original_unit)
				return true;
		}
		// Otherwise do original
		return static_cast<bool(__fastcall*)(engine::hades::Thing*, engine::hades::Thing*)>(original_check_team)(thisptr, other);
	}

	PVOID original_set_effect = nullptr;
	void __fastcall hook_set_effect(engine::hades::Weapon* weapon, const char* effect_name, const char* property_name, engine::misc::any* val, engine::Reflection_ValueChangeType change_type)
	{
		printf("Set effect\n");
		//printf("Replicated: 0x%p\n", global::replicated_unit);
		static ptr any_type = Memory::SigScan("48 83 EC 38 48 C7 44 24 ? ? ? ? ? 48 8B 41 20 48 85 C0 74 12");
		static ptr any_float = Memory::GetCallAddress("E8 ? ? ? ? E9 ? ? ? ? 48 8D 53 10");
		auto player_manager = engine::hades::PlayerManager::Instance();
		auto controllable_unit = (engine::hades::Unit*)player_manager->players[0]->active_unit;

		if (global::replicated_unit && weapon && weapon->pData && weapon->mOwnerId == controllable_unit->mId)
		{
			auto* repl_weapon = global::replicated_unit->arsenal.GetWeapon(weapon->pData->name);
			if (repl_weapon)
			{
				repl_weapon->SetEffectProperty(effect_name, property_name, val, change_type);
			}
		}
		/*if (weapon && weapon->pData)
		{
			printf("weapon: %s\n", weapon->pData->name.ToString());
			printf("Effect: %s:%s\n", effect_name, property_name);
			const type_info* val_type = reinterpret_cast<const type_info * (__fastcall*)(engine::misc::any*)>(any_type)(val);
			printf("Type: 0x%p %s\n", change_type, val_type->name());
			if(val->is_float())
				printf("Val: %f\n", *val->get_float());
		}*/
		return static_cast<void(__fastcall*)(engine::hades::Weapon*, const char*, const char*, engine::misc::any*, engine::Reflection_ValueChangeType)>(original_set_effect)
			(weapon, effect_name, property_name, val, change_type);
	}

	PVOID original_set_data = nullptr;
	void __fastcall hook_set_data(engine::hades::Weapon* weapon, const char* name, engine::misc::any* val, int change_type)
	{
		//printf("Replicated: 0x%p\n", global::replicated_unit);

		/*if (weapon && weapon->pData)
		{
			printf("Weapon: %s\n", weapon->pData->name.ToString());
			printf("Property: %s\n", name);
			printf("Type: 0x%p\n", change_type);
			if (val->is_float())
				printf("Val: %f\n", *val->get_float());
			//printf("Val: 0x%p\n", *val);
		}*/
		return static_cast<void(__fastcall*)(engine::hades::Weapon*, const char*, engine::misc::any*, int)>(original_set_data)(weapon, name, val, change_type);
	}

	PVOID original_set_projectile_property = nullptr;
	void __fastcall hook_set_projectile_property(engine::hades::Weapon* weapon, const char* name, engine::misc::any* val, engine::Reflection_ValueChangeType change_type)
	{
		printf("Projectile Property\n");
		printf("\t%s\n", name);
		if (val->is_float())
			printf("\t%f\n", *val->get_float());

		auto player_manager = engine::hades::PlayerManager::Instance();
		auto controllable_unit = (engine::hades::Unit*)player_manager->players[0]->active_unit;

		if (global::replicated_unit && weapon && weapon->pData && weapon->mOwnerId == controllable_unit->mId)
		{
			auto* repl_weapon = global::replicated_unit->arsenal.GetWeapon(weapon->pData->name);
			if (repl_weapon)
			{
				printf("replicating\n");
				repl_weapon->SetProjectileProperty(name, val, change_type);
			}
		}

		return static_cast<void(__fastcall*)(engine::hades::Weapon*, const char*, engine::misc::any*, engine::Reflection_ValueChangeType)>(original_set_projectile_property)
			(weapon, name, val, change_type);
	}


	PVOID original_find_lock = nullptr;
	DWORD64 __fastcall hook_find_lock(PVOID playerinput, float max_range, float max_arc_dist, float custom_angle, bool can_target_units, bool can_target_friends, bool require_line_of_sight, const char* ignore_name)
	{
		//seed = 2002100;
		//id = 0;
		return static_cast<DWORD64(__fastcall*)(PVOID, float,float,float,bool,bool,bool, const char*)>(original_find_lock)
			(playerinput, max_range * 2.f, max_arc_dist * 4.f, 75.f, true,false,false, ignore_name);
	}

	PVOID original_get_crit = nullptr;
	float __fastcall hook_get_crit_chance(engine::hades::Unit* unit, float base_chance, engine::hades::Unit* target)
	{
		if(unit == engine::hades::PlayerManager::Instance()->players[0]->active_unit)
			return 1.f;
		return static_cast<float(__fastcall*)(engine::hades::Unit*, float, engine::hades::Unit*)>(original_get_crit)(unit, base_chance, target);
	}

	PVOID original_create_enemy_unit = nullptr;
	engine::hades::Unit* __fastcall hook_create_enemy_unit(engine::hades::UnitData* unit_data, D3DXVECTOR2 location, engine::hades::MapThing* map_thing, std::vector<engine::hades::MapThingGroupId>* group_names,
		const engine::hades::MapThingGroupId* first_group_name, uint8_t do_active_presentation, bool needs_lua_init, engine::hades::Unit* spawned_by)
	{
		bool is_host = network::is_host;
		/* Get original unit created */
		engine::hades::Unit* unit = reinterpret_cast<engine::hades::Unit * (__fastcall*)(engine::hades::UnitData*, D3DXVECTOR2, engine::hades::MapThing*, std::vector<engine::hades::MapThingGroupId>*,
			const engine::hades::MapThingGroupId*, bool, bool, engine::hades::Unit*)>(original_create_enemy_unit)(unit_data, location, map_thing, group_names,
				first_group_name, do_active_presentation, needs_lua_init, spawned_by);
		if (is_host)
		{
			printf("Spawning enemy unit: %s\n", unit_data->name.ToString());

			/* Send enemy spawn packet */
			msgpack::zone z;
			network::packets::AddEnemyPacket add_enemy_packet(unit, location);
			network::packets::Packet packet(network::packets::PacketType::OnAddEnemy, msgpack::object(add_enemy_packet, z));
			std::stringstream ss;
			msgpack::pack(ss, packet);
			network::client.Send(ss.str());
		}
		else
		{
			if(do_active_presentation != 2) // This entity was not created by server.
				unit->Delete();// Delete unit if not host, await server entity creation packet
		}
		return unit;
	}

	PVOID original_handle_input = nullptr;
	void __fastcall hook_handle_input(PVOID thisptr, float dt, PVOID input_handler)
	{
		printf("Input %f\n", dt);
		printf("\t0x%p\n", input_handler);
		return reinterpret_cast<void(__fastcall*)(PVOID, float, PVOID)>(original_handle_input)(thisptr, dt, input_handler);
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

		MH_CreateHook((PVOID)engine::addresses::weapon::functions::request_fire, &hook_request_fire, &original_request_fire);
		MH_EnableHook((PVOID)engine::addresses::weapon::functions::request_fire);

		MH_CreateHook((PVOID)engine::addresses::world::functions::load_next_map, &hook_load_next_map, &original_load_next_map);
		MH_EnableHook((PVOID)engine::addresses::world::functions::load_next_map);

		MH_CreateHook((PVOID)engine::addresses::gameplayscreen::functions::draw, &hook_gameplayscreen_draw, &original_draw);
		MH_EnableHook((PVOID)engine::addresses::gameplayscreen::functions::draw);

		/*MH_CreateHook((PVOID)engine::addresses::weapon::functions::set_effect_property, &hook_set_effect, &original_set_effect);
		MH_EnableHook((PVOID)engine::addresses::weapon::functions::set_effect_property);

		MH_CreateHook((PVOID)engine::addresses::weapon::functions::set_data_property, &hook_set_data, &original_set_data);
		MH_EnableHook((PVOID)engine::addresses::weapon::functions::set_data_property);

		MH_CreateHook((PVOID)engine::addresses::weapon::functions::set_projectile_property, &hook_set_projectile_property, &original_set_projectile_property);
		MH_EnableHook((PVOID)engine::addresses::weapon::functions::set_projectile_property);*/

		MH_CreateHook((PVOID)engine::addresses::unitmanager::functions::create_enemy_unit, &hook_create_enemy_unit, &original_create_enemy_unit);
		MH_EnableHook((PVOID)engine::addresses::unitmanager::functions::create_enemy_unit);

		//MH_CreateHook((PVOID)engine::addresses::playerunit::functions::handle_input, &hook_handle_input, &original_handle_input);
		//MH_EnableHook((PVOID)engine::addresses::playerunit::functions::handle_input);

		PVOID hook_address = (PVOID)Memory::GetCallAddress("E8 ? ? ? ? 48 85 C0 74 1D 48 8D 55 B7");
		//MH_CreateHook(hook_address, &hook_find_lock, &original_find_lock);
		//MH_EnableHook(hook_address);
		//PVOID crit_address = (PVOID)Memory::SigScan("48 8B 81 ? ? ? ? 4D 8B D8");
		//MH_CreateHook(crit_address, &hook_get_crit_chance, &original_get_crit);
		//MH_EnableHook(crit_address);
		//MH_CreateHook((PVOID)engine::addresses::unit::functions::has_same_team, &hook_has_same_team, &original_check_team);
		//MH_EnableHook((PVOID)engine::addresses::unit::functions::has_same_team);
		// Script load hook

		// Prevent unloading
		uintptr_t address = (uintptr_t)GetModuleHandleA("SDL2.dll") + 0x00ED561;
		printf("Address: 0x%p\n", address);
		uint8_t nop[] = { 0x90,0x90,0x90,0x90,0x90,0x90 };
		DWORD protect;
		VirtualProtect((PVOID)address, 6, PAGE_EXECUTE_READWRITE, &protect);
		memcpy((PVOID)address, nop, 6);
		VirtualProtect((PVOID)address, 6, protect, &protect);

		// DirectX hook
		//
	}
}