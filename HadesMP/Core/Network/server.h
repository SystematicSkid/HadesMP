#include "defines.h"

#include <iostream>
#include <string>
#include <thread>

#define ENET_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <enet/include/enet.h>
#include <enetpp/include/server.h>

namespace core::network::server
{
	auto trace_handler = [&](const std::string& msg)
	{
		std::cout << "server: " << msg << std::endl;
	};

	struct server_client
	{
		unsigned int _uid;
		unsigned int get_uid() const { return _uid; } //MUST return globally unique value here
	};

	unsigned int next_uid = 1;
	auto init_client_func = [&](server_client& client, const char* ip)
	{
		client._uid = next_uid;
		next_uid++;
	};

	bool server_init(int port)
	{
		/*if (enet_initialize() != 0)
			return false;

		ENetAddress address;
		enet_address_set_host(&address, "localhost");
		address.port = port;

		ENetHost* host = enet_host_create(&address, max_players, 2, 0, 0);
		if (!host)
			return false;*/

		enetpp::server<server_client> server;
		server.set_trace_handler(trace_handler);

		server.start_listening(enetpp::server_listen_params<server_client>()
			.set_max_client_count(20)
			.set_channel_count(1)
			.set_listen_port(port)
			.set_initialize_client_function(init_client_func));

		std::vector<uint8_t> buf;
		core::network::packet::anime obj{ {20, 20}, cista::raw::string{"test"} };
		buf = cista::serialize(obj);

		while (server.is_listening())
		{
			//send stuff to specific client where uid=123

			//servertrace_handler(std::to_string((DWORD64)buf.data()).c_str());

			server.send_packet_to(1, 0, buf.data(), buf.size(), ENET_PACKET_FLAG_RELIABLE);

			//send stuff to all clients (with optional predicate filter)
			//server.send_packet_to_all_if(0, &data_to_send, 1, ENET_PACKET_FLAG_RELIABLE, [&](const server_client& destination) { return true; });

			static auto on_connected = [=](server_client& client)
			{
				trace_handler("server_on_connected");

				//connected = true;
			};

			static auto on_disconnected = [=](unsigned int client_uid)
			{
				trace_handler("server_on_disconnected");
			};

			static auto on_data_received = [=](server_client& client, const enet_uint8* data, size_t data_size)
			{
				trace_handler("server_on_datarecv");

				std::vector<uint8_t> serialized_data(&data[0], &data[data_size]);
				auto deserialized_data = cista::deserialize<core::network::packet::anime>(serialized_data);

				trace_handler(deserialized_data->msg.str());
			};

			server.consume_events(
				on_connected,
				on_disconnected,
				on_data_received);

			//get access to all connected clients
			for (auto c : server.get_connected_clients())
			{
				//do something?
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}