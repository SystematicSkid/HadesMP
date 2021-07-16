// win
#include <WinSock2.h>
#include <Windows.h>
#include <winternl.h>
#include <Xinput.h>
#include <d3d11.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <D3DX10math.h>
#include <minwindef.h>
#include <winbase.h>
#include <intrin.h>
#include <psapi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

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
#include <concurrent_vector.h>


/* Engine Access */
#include "../Engine/engine.h"

/* Utilities */
#include "../Utilities/memory.h"
#include "../Utilities/proxy.h"

/* Networking */
#include "./Network/packets.h"
#include "./Network/network.h"
#include "./Network/client.h"
#include "./Network/server.h"

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

	int tick = 0;

	void proxy_thread()
	{
		auto client = network::client.Init();
		auto server = network::server.Init();
		server->join();
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
			text.draw_text(&g, { 120, 30 }, "Hades Online - v0.5", { 255,255,255,255 }, TEXT_CENTERED | TEXT_OUTLINE, { 0,0,0,127 });

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
					if (unit->mSpawnLocation == unit->location)
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
			/* Update global dt */
			global::delta_time = dt;

			/* Initialize our network once */
			std::call_once(network_init_flag, [=]()
				{
					/* Initialize all here */
					printf("Creating thread...\n");
					hooks::directx::initialize();
					CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)proxy_thread, NULL, NULL, NULL);
					printf("Finished creating thread!\n");
				});

			auto player_manager = engine::hades::PlayerManager::Instance();
			auto controllable_unit = (engine::hades::Unit*)player_manager->players[0]->active_unit;
			auto world = engine::hades::World::Instance;
			app->mIsActive = true;
			app->mWasActive = true;
			if (world)
			{
				world->mPaused = false;
				world->mRunning = true;
			}

			// We need to verify that the lua handler has already assigned our player's active unit
			if (global::new_map_thing && global::new_unit_data && player_manager && player_manager->players[0]->active_unit && !global::replicated_unit)
			{
				printf("\n\nCreating new unit!\n");
				printf("Location: %f %f\n", global::spawn_location.x, global::spawn_location.y);
				printf("Data: 0x%p\n", global::new_unit_data);
				printf("Thing: 0x%p\n", global::new_map_thing);

				printf("Unit data: 0x%p\n", global::new_unit_data->mPlayerUnitData);
				printf("Playermanager: 0x%p\n", engine::hades::PlayerManager::Instance());
				//global::new_unit_data->mPlayerUnitData->mPlayerIndex++;

				global::replicated_unit = engine::hades::UnitManager::CreatePlayerUnit(global::new_unit_data, global::spawn_location, global::new_map_thing, false, false);
				//engine::hades::PlayerManager::Instance()->AssignController((engine::hades::Player*)global::replicated_unit, 1);
				global::replicated_unit->CopyArsenal(controllable_unit);
				global::replicated_unit->pData->mPlayerControlled = false;
				//engine::hades::PlayerManager::Instance()->players[1]->active_unit = global::replicated_unit;
				//engine::hades::PlayerManager::Instance()->players[1]->controller_index = 4;
				engine::misc::Color col(255, 0, 0, 255);
				world->AddOutline(global::replicated_unit, col, 1.f, 2.f, 0.5f, -1.f, true, true);
				printf("0x%p\n", engine::hades::World::Instance);

				printf("unit: 0x%p\n", global::replicated_unit);

				// Fixup some stuff

				// Copy the lua table over
				global::replicated_unit->mAttachedLua.state = controllable_unit->mAttachedLua.state;
				global::replicated_unit->mAttachedLua.ref = controllable_unit->mAttachedLua.ref;
			}

			
			if (global::replicated_unit && controllable_unit)
			{
				// Send move command
				if (global::move_pos != D3DXVECTOR2(0, 0))
				{
					if(global::replicated_unit->Distance(global::move_pos) > 25.f)
						global::replicated_unit->MoveInput(&global::move_pos, 1.0, false, dt);
				}

			}

			if (controllable_unit)
			{
				msgpack::zone z;
				/* Send movement packet */
				network::packets::MovementPacket movement_packet(controllable_unit);
				network::packets::Packet packet(network::packets::PacketType::OnMovementUpdate, msgpack::object(movement_packet, z));
				std::stringstream ss;
				msgpack::pack(ss, packet);
				network::client.Send(ss.str());

				/* Send Input packet */
				/*msgpack::zone z;
				network::packets::PlayerInputPacket input_packet(engine::hades::PlayerManager::Instance()->inputs[0]);
				network::packets::Packet packet(network::packets::PacketType::OnPlayerInput, msgpack::object(input_packet, z));
				std::stringstream ss;
				msgpack::pack(ss, packet);
				network::client.Send(ss.str());*/
			}

			/* Create network players if needed */
			/*if(controllable_unit)
			{
				for (auto player : network::network_players)
				{
					std::string res;
					auto map_name = world->GetMap()->GetName(&res);
					if (!player->character && strcmp(map_name->c_str(), player->map.c_str()) == 0) // If they do not have a character created yet, but we are on the same map
					{
						player->character = engine::hades::UnitManager::CreatePlayerUnit(global::new_unit_data, global::spawn_location, global::new_map_thing, false, false);
						world->AddOutline(player->character, player->outline_color, 1.f, 2.f, 0.5f, -1.f, true, true);
						player->character->mAttachedLua.state = controllable_unit->mAttachedLua.state;
						player->character->mAttachedLua.ref = controllable_unit->mAttachedLua.ref;
					}

					if (player->character && strcmp(map_name->c_str(), player->map.c_str()) != 0)
					{
						player->character->Delete();
						player->character = nullptr;
					}

					if (player->character)
					{
						// If their character exists, we can do stuff with it
						if (player->character->Distance(player->network_position) > 25.f) // Only send move command if they aren't standing still.
						{
							player->character->MoveInput(&player->network_position, 1.f, false, true);
						}
						/ *auto unit = (engine::hades::Unit*)player->character;
						// Check if we are missing any weapons or have too many!
						if (player->weapons.size() != unit->GetArsenal()->mWeapons.size())
						{
							// Reset arsenal
							unit->GetArsenal()->mWeapons.clear();
							for (auto id : player->weapons)
							{
								auto hash = engine::misc::HashGuid(id);
								unit->GetArsenal()->AddWeapon(hash);
							}
						}* /
					}
				}
			}*/
			
			/* Send network information */
			/* Ensure we are connected */
			/*if(network::client::uuid != -1)
			{
				network::json j;
				j["uid"] = network::client::uuid;
				j["time"] = engine::hades::World::Instance->mElapsedTime;
				j["tick"] = global::tick++;

				//j["has_unit"] = (bool)(controllable_unit != nullptr);
				/ * Player Specific information * /
				j["player_position_x"] = 0.f;
				j["player_position_y"] = 0.f;
				j["world_map"] = "";
				std::vector<int> weapons;
				j["player_arsenal"] = weapons;
				if (controllable_unit)
				{
					float pos_x = controllable_unit->location.x;
					float pos_y = controllable_unit->location.y;
					j["player_position_x"] = (float)pos_x;
					j["player_position_y"] = (float)pos_y;
					/ * World Specific information* /
					/ * This won't exist without the player regardless * /
					//j["has_world"] = (world != nullptr && world->GetMapData() != nullptr);
					if (world && world->mMap.mData)
					{
						std::string res;
						j["world_map"] = world->GetMap()->GetName(&res)->c_str();
					}

					auto unit = (engine::hades::Unit*)controllable_unit;
					weapons = unit->GetArsenal()->GetWeapons();
					j["player_arsenal"] = weapons;
				}
				core::network::client::Send("OnTick", j);
			}*/
		};



		/* Network callbacks */

		network::client.callbacks.push_back(network::NetworkCallback(network::packets::PacketType::OnMovementUpdate, [=](std::string data)
			{
				/* Setup packet from data stream */
				msgpack::object_handle handle = msgpack::unpack(data.data(), data.size());
				msgpack::object msg = handle.get();
				network::packets::Packet base_packet = msg.as<network::packets::Packet>();

				network::packets::MovementPacket packet = base_packet.data.as<network::packets::MovementPacket>();

				
				if (global::replicated_unit)
				{
					global::replicated_unit->mIsMoving = packet.is_moving;
					//global::replicated_unit->SetAngle(packet.angle);
					global::move_pos.x = packet.location_x;
					global::move_pos.y = packet.location_y;
				}
			}));

		network::client.callbacks.push_back(network::NetworkCallback(network::packets::PacketType::OnPlayerInput, [=](std::string data)
			{
				/* Setup packet from data stream */
				msgpack::object_handle handle = msgpack::unpack(data.data(), data.size());
				msgpack::object msg = handle.get();
				network::packets::Packet base_packet = msg.as<network::packets::Packet>();

				network::packets::PlayerInputPacket packet = base_packet.data.as<network::packets::PlayerInputPacket>();


				if (global::replicated_unit)
				{
					engine::hades::PlayerManager::Instance()->inputs[4]->SetButtons(packet.buttons);
				}
			}));

		network::client.callbacks.push_back(network::NetworkCallback(network::packets::PacketType::OnAddEnemy, [=](std::string data)
			{
				/* Setup packet from data stream */
				msgpack::object_handle handle = msgpack::unpack(data.data(), data.size());
				msgpack::object msg = handle.get();
				network::packets::Packet base_packet = msg.as<network::packets::Packet>();

				network::packets::AddEnemyPacket packet = base_packet.data.as<network::packets::AddEnemyPacket>();


				if (global::replicated_unit)
				{
					engine::misc::HashGuid unit_hash(packet.enemy_name.c_str());
					engine::hades::UnitData* unit_data = engine::hades::GameDataManager::GetUnitData(unit_hash);
					D3DXVECTOR2 location(packet.location_x, packet.location_y);
					auto* unit = engine::hades::UnitManager::CreateEnemyUnit(unit_data, location);
					unit->mId = packet.id;
				}
			}));

		network::client.callbacks.push_back(network::NetworkCallback(network::packets::PacketType::OnAttack, [=](std::string data)
			{
				/* Setup packet from data stream */
				msgpack::object_handle handle = msgpack::unpack(data.data(), data.size());
				msgpack::object msg = handle.get();
				network::packets::Packet base_packet = msg.as<network::packets::Packet>();

				network::packets::AttackPacket packet = base_packet.data.as<network::packets::AttackPacket>();


				if (global::replicated_unit)
				{
					engine::hades::Unit* unit = global::replicated_unit;
					engine::misc::HashGuid weapon_hash(packet.weapon_hash.c_str());
					engine::hades::Weapon* weapon = unit->arsenal.GetWeapon(weapon_hash);
					printf("Packet weapon: %i\n", packet.weapon_hash);
					printf("Packet %s\n", weapon_hash.ToString());
					if (!weapon)
					{
						unit->arsenal.AddWeapon(engine::hades::Weapon::Create(weapon_hash, unit));
						weapon = unit->arsenal.GetWeapon(weapon_hash);
					}

					engine::hades::Thing* target = nullptr;
					if (packet.target_name.length() > 0)
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
							if (unit->mSpawnLocation == unit->location)
								continue;
							engine::misc::HashGuid tmp;
							auto name = unit->GetName(&tmp);
							std::string str_name = name->ToString();
							if (str_name == packet.target_name)
							{
								target = unit;
								break;
							}
						}
					}

					if (weapon)
					{
						printf("Requesting...\n");
						weapon->mReloadPaused = packet.reload_paused;
						weapon->mRequireControlRelease = packet.require_control_release;
						weapon->mCharging = packet.charging;
						weapon->mLastFireTime = packet.last_fire_time;
						weapon->mControlTime = packet.control_time;
						weapon->mCooldownTimeRemaining = packet.cooldown_time_remaining;
						weapon->mAmmo = packet.ammo;
						weapon->mVolley = packet.volley;
						weapon->mReloadTimeRemaining = packet.reload_time_remaining;
						weapon->RequestFire(packet.angle, D3DXVECTOR2(packet.target_position_x, packet.target_position_y), target, packet.apply_self_effects, D3DXVECTOR2(packet.from_position_x, packet.from_position_y));
					}
				}
			}));

		

		/* Everything was initialized correctly! */
		return true;
	}


}