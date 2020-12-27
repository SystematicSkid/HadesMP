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

	enetpp::trace_handler trace_handler;
	enetpp::client client;
	std::mutex mutex;
	concurrency::concurrent_vector<json> queue;
	concurrency::concurrent_vector<NetworkCallback> callbacks;
	int uuid = -1;


	void Consume()
	{
		static auto on_connected = [=]()
		{
			printf("Connected!\n");
			trace_handler("on_connected");

			//connected = true;
		};

		static auto on_disconnected = [=]()
		{
			trace_handler("on_disconnected");
		};

		static auto on_data_received = [=](const enet_uint8* data, size_t data_size)
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

		client.consume_events(on_connected, on_disconnected, on_data_received);
	}

	void Invoke()
	{
		while (client.is_connecting_or_connected())
		{
			//if (connected)
			{
				for (auto& msg : queue)
				{
					auto cbor = json::to_cbor(msg);

					client.send_packet(0, reinterpret_cast<const enet_uint8*>(cbor.data()), cbor.size(), ENET_PACKET_FLAG_RELIABLE);
				}

				queue = concurrency::concurrent_vector<json>{};
				//queue.clear();
			}

			Consume();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	void Connect(const std::string& hostname, enet_uint16 port)
	{
		client.connect(enetpp::client_connect_params()
			.set_channel_count(1)
			.set_server_host_name_and_port(hostname.c_str(), port));
	}

	void Thread()
	{
		enetpp::global_state::get().initialize();

		trace_handler = [&](const std::string& msg)
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << "client: " << msg << "\n";
		};

		client.set_trace_handler(trace_handler);

		Connect("localhost", 27017);

		Invoke();

		std::terminate();
	}

	std::unique_ptr<std::thread> Init()
	{
		return std::make_unique<std::thread>(&client::Thread);
	}


	void Disconnect()
	{
		client.disconnect();
	}

	void Send(const std::string& to, json message)
	{
		if(client.is_connecting_or_connected())
			queue.push_back(json{ to, message });
	}

	void Send(const std::string& to, const std::string& message)
	{
		if (client.is_connecting_or_connected())
			queue.push_back(json{ to, message });
	}

	void OnSync(const std::string& name, std::function<void(json)> function)
	{
		std::promise<json> promise;

		callbacks.push_back(NetworkCallback(name, [&](json message) { promise.set_value(message); }));

		std::future<json> future = promise.get_future();

		function(future.get());
	}
}