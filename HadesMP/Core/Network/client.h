#include "defines.h"

#include <iostream>
#include <string>
#include <thread>

#define ENET_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <enet/include/enet.h>
#include <enetpp/include/client.h>

namespace core::network::client
{
	auto trace_handler = [&](const std::string& msg)
	{
		std::cout << "client: " << msg << std::endl;
	};

	bool client_init(int port)
	{
		while (true)
		{
			enetpp::client client;
			client.set_trace_handler(trace_handler);
			client.connect(enetpp::client_connect_params()
				.set_channel_count(1)
				.set_server_host_name_and_port("localhost", port));

			std::vector<uint8_t> buf;
			core::network::packet::anime obj{ {20, 20}, cista::raw::string{"gamer moment? xd"} };
			buf = cista::serialize(obj);

			while (client.is_connecting_or_connected())
			{
				//enet_uint8 data_to_send = 1;
				//client.send_packet(0, reinterpret_cast<const enet_uint8*>(data_to_send), 1, ENET_PACKET_FLAG_RELIABLE);

				//enet_uint8 data_to_send;
				//client.send_packet(0, &data_to_send, 1, ENET_PACKET_FLAG_RELIABLE);
				auto player_manager = engine::hades::PlayerManager::Instance();
				auto controllable_unit = player_manager->players[0]->active_unit;
				if (controllable_unit)
				{
					auto coords = controllable_unit->location;
					core::network::packet::anime obj{ {coords.x, coords.y}, cista::raw::string{"gamer moment? xd"} };
					buf = cista::serialize(obj);

					client.send_packet(0, buf.data(), buf.size(), ENET_PACKET_FLAG_RELIABLE);
				}

				static auto on_connected = [=]()
				{
					//trace_handler("client_on_connected");

					//connected = true;
				};

				static auto on_disconnected = [=]()
				{
					//trace_handler("client_on_disconnected");
				};

				static auto on_data_received = [=](const enet_uint8* data, size_t data_size)
				{
					//trace_handler("client_on_datarecv");
					auto player_manager = engine::hades::PlayerManager::Instance();
					auto controllable_unit = player_manager->players[0]->active_unit;
					if(global::replicated_unit && controllable_unit)
					{
						std::vector<uint8_t> serialized_data(&data[0], &data[data_size]);
						auto deserialized_data = cista::deserialize<core::network::packet::anime>(serialized_data);
						D3DXVECTOR2 pos = D3DXVECTOR2(deserialized_data->pos.x, deserialized_data->pos.y);
						float dist = global::replicated_unit->Distance(pos);

						if (dist > 10.f)
							global::replicated_unit->MoveInput(&pos, 1.0f, false, global::delta_time);
					}
				};

				client.consume_events(
					on_connected,
					on_disconnected,
					on_data_received);

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}