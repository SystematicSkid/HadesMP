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
	enetpp::trace_handler trace_handler;
	enetpp::server<server_client> server;
	std::mutex mutex;
	concurrency::concurrent_vector<json> queue;
	concurrency::concurrent_vector<NetworkCallback> callbacks;


	unsigned int next_uid = 1;
	inline auto init_client_func = [&](server_client& client, const char* ip)
	{
		client._uid = next_uid;
		next_uid++;
	};


	void StartListening(const std::string& hostname, enet_uint16 port)
	{
		server.start_listening(enetpp::server_listen_params<server_client>()
			.set_channel_count(1)
			.set_max_client_count(0xFFF)
			.set_incoming_bandwidth(0)
			.set_outgoing_bandwidth(0)
			.set_listen_port(port)
			.set_initialize_client_function(init_client_func));
	}

	void Send(int id, const std::string& to, json message)
	{
		queue.push_back(json{ id, json{ to, message } });
	}
	void Send(int id, const std::string& to, const std::string& message)
	{
		queue.push_back(json{ id, json{ to, message } });
	}

	void Consume()
	{
		static auto on_connected = [=](server_client& client)
		{
			trace_handler("on_connected");

			for (auto c : server.get_connected_clients())
				Send(c->get_uid(), "JoinMSGPacket", json{ std::string(std::to_string(client.get_uid()) + " has joined.") });

			//connected = true;
		};

		static auto on_disconnected = [=](unsigned int client_uid)
		{
			trace_handler("on_disconnected");
		};

		static auto on_data_received = [=](server_client& client, const enet_uint8* data, size_t data_size)
		{
			try
			{
				json j_from_cbor = json::from_cbor(data, data + data_size);

				std::string callback_name = j_from_cbor[0];

				json message = j_from_cbor[1];

				for (auto& callback : callbacks)
				{
					if (callback.name == callback_name)
						callback.callback(message);
				}
			}
			catch (std::exception& e)
			{
				MessageBoxA(NULL, e.what(), e.what(), NULL);
			}
		};

		server.consume_events(on_connected, on_disconnected, on_data_received);
	}

	void Invoke()
	{
		while (server.is_listening())
		{
			if (!server.get_connected_clients().empty())
			{
				for (auto& msg : queue)
				{
					auto cbor = json::to_cbor(msg[1]);

					if (cbor.data())
						server.send_packet_to(msg[0], 0, reinterpret_cast<const enet_uint8*>(cbor.data()), cbor.size(), ENET_PACKET_FLAG_RELIABLE);
				}

				queue = concurrency::concurrent_vector<json>{};
				//queue.clear();
			}

			Consume();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	void Thread()
	{
		enetpp::global_state::get().initialize();

		trace_handler = [&](const std::string& msg)
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << "server: " << msg << "\n";
		};

		server.set_trace_handler(trace_handler);

		StartListening("localhost", 27017);

		Invoke();

		std::terminate();
	}

	std::unique_ptr<std::thread> Init()
	{
		return std::make_unique<std::thread>(&Thread);
	}
	
	void StopListening()
	{
		server.stop_listening();
	}
	
	void OnSync(const std::string& name, std::function<void(json)> function)
	{
		std::promise<json> promise;

		callbacks.push_back(NetworkCallback(name, [&](json message) { promise.set_value(message); }));

		std::future<json> future = promise.get_future();

		function(future.get());
	}
}