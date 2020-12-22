// win
#include <WinSock2.h>
#include <Windows.h>
#include <winternl.h>
#include <Xinput.h>
#include <d3d11.h>
#include <d3d9.h>
#include <D3DX10math.h>
#include <minwindef.h>
#include <winbase.h>
#include <intrin.h>
#include <psapi.h>

#pragma comment(lib, "d3d11.lib")

// stl
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
#include <clocale>
#include <algorithm>
#include <sstream>
#include <functional>
#include <cassert>
#include <mutex>
#include <thread>

/* Engine Access */
#include "../Engine/engine.h"

/* Utilities */
#include "../Utilities/memory.h"
#include "../Utilities/proxy.h"

#include "Network/server.h"
#include "Network/client.h"

/* Hooks */
#include "./Hooks/hooks.h"

/* Renderer */
#include "./Renderer/renderer.h"

namespace core
{
	// Variables 
	std::once_flag init_flag;
	std::once_flag network_init_flag;
	ID3D11DeviceContext* pContext;
	ID3D11Device* pDevice;
	font text;
	font world_text;

	void proxy_thread()
	{
		auto client = std::make_unique<std::thread>(&core::network::client::client_init, 27017);
		client->join();
	}

	bool initialize()
	{
		/* Initialize our proxy system */
		if (!proxy.initialize())
			return false;
		/* Initialize all engine hooks -> Possibly add error checking? */
		hooks::initialize();

		/* Initialize all of our callback */

		/* DirectX->Present callback, used for drawing*/
		core::hooks::directx::on_present = [=](IDXGISwapChain* pSwapChain)
		{
			std::call_once(init_flag, [=]()
				{
					printf("[ + ] Initializing renderer...\n");
					pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice);
					pDevice->GetImmediateContext(&pContext);
					renderer_setup(pDevice, pContext);

					text.create("Tahoma", 14, FONT_DEFAULT);
					world_text.create("Tahoma", 8, FONT_DEFAULT);
				});

			renderer_start_drawing();
			geometry g;
			g.push_draw_cmd();
			text.draw_text(&g, { 120, 30 }, "Hades Online - v0.2", { 255,255,255,255 }, TEXT_CENTERED | TEXT_OUTLINE, { 0,0,0,127 });

			auto player_manager = engine::hades::PlayerManager::Instance();
			auto controllable_unit = player_manager->players[0]->active_unit;
			if (controllable_unit)
			{

				auto units = engine::hades::UnitManager::GetAllUnits();
				for (int i = 0; i < engine::hades::UnitManager::GetNumUnits(); i++)
				{
					auto unit = units[i];
					if (!unit)
						continue;
					if (!(unit->mType & engine::Unit))
						continue;
					if (!unit->IsTargetable())
						continue;
					if (unit->spawn_location == unit->location)
						continue;
					engine::misc::HashGuid tmp;
					auto name = unit->GetName(&tmp);
					float parallax = unit->GetParallaxAmount();
					D3DXVECTOR2 screen;
					engine::hades::Camera::Instance()->WorldToScreenParallax(&unit->location, &screen, parallax);
					world_text.draw_text(&g, { screen.x, screen.y }, name->ToString(), { 255,255,255,255 }, TEXT_CENTERED | TEXT_OUTLINE, { 0,0,0,127 });
				}
			}

			g.render();
			renderer_draw();
		};

		/* Create our `OnUpdate` callback */
		core::hooks::on_update = [=](engine::App* app, float dt)
		{
			/* Initialize our network once */
			std::call_once(network_init_flag, [=]()
				{
					/* Initialize all here */
					printf("Creating thread...\n");
					CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)proxy_thread, NULL, NULL, NULL);
					printf("Finished creating thread!\n");
				});

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
				engine::misc::Color col(255, 0, 0, 255);
				engine::hades::World::Instance->AddOutline(global::replicated_unit, col, 1.f, 2.f, 0.5f, -1.f, true, true);
				printf("0x%p\n", engine::hades::World::Instance);

				printf("unit: 0x%p\n", global::replicated_unit);

				// Fixup some stuff

				// Copy the lua table over
				global::replicated_unit->mAttachedLua.state = controllable_unit->mAttachedLua.state;
				global::replicated_unit->mAttachedLua.ref = controllable_unit->mAttachedLua.ref;
			}

			/*if (global::replicated_unit)
			{
				if (player_manager)
				{
					auto player_unit = player_manager->players[0]->active_unit;
					if (player_unit && global::replicated_unit)
					{

						auto get_closest_enemy = [=](engine::hades::Thing* localent)
						{
							float closest_dist = FLT_MAX;
							engine::hades::Thing* ret = nullptr;
							auto units = engine::hades::UnitManager::GetAllUnits();
							for (int i = 0; i < engine::hades::UnitManager::GetNumUnits(); i++)
							{
								auto unit = units[i];
								if (!unit)
									continue;
								engine::misc::HashGuid tmp;
								auto name = unit->GetName(&tmp);
								if (unit == controllable_unit)
									continue;

								if (unit == localent)
									continue;
								if (!(unit->mType & engine::Unit))
									continue;

								if (!unit->IsTargetable())
									continue;
								if (unit->spawn_location == unit->location)
									continue;

								if (localent->Distance(unit) < closest_dist)
								{
									closest_dist = localent->Distance(unit);
									ret = unit;
								}
							}
							return ret;
						};

						engine::hades::PlayerUnitInput* input = *(engine::hades::PlayerUnitInput**)((uintptr_t)global::replicated_unit + 0x560);
						D3DXVECTOR2 target_location = player_unit->location;
						D3DXVECTOR2 move_location = D3DXVECTOR2();
						auto target = get_closest_enemy(global::replicated_unit);
						auto unit_type = (engine::hades::Unit*)global::replicated_unit;
						if (target && unit_type->arsenal.mWeapons.size() > 3)
						{
							//printf("Dist: %f\n", global::replicated_unit->Distance(target));
							engine::hades::Weapon* dash = unit_type->arsenal.mControllableWeapons[0];
							if (global::replicated_unit->Distance(target) > 200.f && dash && dash->CanFire())
							{
								auto angle = global::replicated_unit->GetAngle(target);
								dash->RequestFire(angle, target->location, target);
							}
							engine::hades::Weapon* special = unit_type->arsenal.mControllableWeapons[3];
							if (global::replicated_unit->Distance(target) < 100.f && special && special->CanFire())
							{
								auto angle = global::replicated_unit->GetAngle(target);
								special->RequestFire(angle, target->location, target);
							}
							//engine::hades::PathFinder::CalcPath(global::replicated_unit, target_location, player_unit, 10.f / *Success Dist* /);
							//input->pPlayerPathfinding->GetNextPathStep(&move_location, target_location);
							global::replicated_unit->MoveInput(&target->location, 1.0, false, dt);
						}
					}
				}
			}
*/

			if (global::replicated_unit && controllable_unit)
			{

				/* W2S Test */
				float parallax = controllable_unit->GetParallaxAmount();
				D3DXVECTOR2 screen;
				engine::hades::Camera::Instance()->WorldToScreenParallax(&controllable_unit->location, &screen, parallax);
				//printf("%f %f\n", screen.x, screen.y);

				auto unit_type = (engine::hades::Unit*)global::replicated_unit;
				auto unit_type_original = (engine::hades::Unit*)controllable_unit;
				//unit_type->status.SetAsAlly();

				// The replicated unit does not have any weapons, create them!
				if (unit_type->arsenal.mWeapons.size() < 3)
					unit_type->CopyArsenal((engine::hades::Unit*)controllable_unit);

			};

		};

		/* Everything was initialized correctly! */
		return true;
	}
}