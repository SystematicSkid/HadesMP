#pragma once

#include <iostream>
#include <string>
#include <thread>

#define ENET_IMPLEMENTATION

#include <enet/include/enet.h>
#include <enetpp/include/client.h>

namespace core::network
{

	void clientthreadfunc()
	{
		client.Thread();
	}

	std::unique_ptr<std::thread> Client::Init()
	{
		return std::make_unique<std::thread>(&clientthreadfunc);
	}

	void Client::Thread()
	{
		enetpp::global_state::get().initialize();

		trace_handler = [&](const std::string& msg)
		{
			std::lock_guard<std::mutex> lock(mutex);
			printf("client: %s\n", msg.c_str());
		};

		client.set_trace_handler(trace_handler);

		Connect("localhost", 27017);

		Invoke();

		std::terminate();
	}

	void Client::Connect(const std::string& hostname, enet_uint16 port)
	{
		client.connect(enetpp::client_connect_params()
			.set_channel_count(1)
			.set_server_host_name_and_port(hostname.c_str(), port));
	}

	void Client::Disconnect()
	{
		//connected = false;

		client.disconnect();
	}

	void Client::Send(std::string message)
	{
		queue.push_back(message);
	}

	void Client::Consume()
	{
		static auto on_connected = [this]()
		{
			trace_handler("on_connected");

			//connected = true;
		};

		static auto on_disconnected = [this]()
		{
			trace_handler("on_disconnected");
		};

		static auto on_data_received = [this](const enet_uint8* data, size_t data_size)
		{
			try
			{
				std::string datastring((char*)data, data_size);
				msgpack::object_handle handle = msgpack::unpack(datastring.data(), datastring.size());
				msgpack::object msg = handle.get();

				packets::Packet base_packet = msg.as<packets::Packet>();

				//printf("[Client] Received packet: %i\n", base_packet.type);

				for (auto& callback : callbacks)
				{
					if (callback.type == (packets::PacketType)base_packet.type)
						callback.callback(datastring);
				}


				/*std::string callback_name = std::get<0>(prototype);
				std::string data = std::to_string(std::get<1>(prototype));

				*/
			}
			catch (std::exception& e)
			{
				printf("Client error\n");
				printf("[Client] Bytes: ");
				for (int i = 0; i < data_size; i++)
					printf("0x%02X ", data[i]);
				printf("\n");
				//MessageBoxA(NULL, e.what(), e.what(), NULL);
			}
		};

		client.consume_events(on_connected, on_disconnected, on_data_received);
	}

	// on sync will wait til it receives some sort of response (blocking)
	void Client::OnSync(const std::string& name, std::function<void(std::string)> function)
	{
		/*std::promise<json> promise;

		callbacks.push_back(NetworkCallback(name, [&](json message) { promise.set_value(message); }));

		std::future<json> future = promise.get_future();

		function(future.get());*/
	}

	void Client::Invoke()
	{
		while (client.is_connecting_or_connected())
		{
			//if (connected)
			{
				for (auto& packet : queue)
				{
					if (!packet.empty())
						client.send_packet(0, reinterpret_cast<const enet_uint8*>(packet.data()), packet.size(), ENET_PACKET_FLAG_RELIABLE);
				}

				queue.clear();
			}

			Consume();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	extern Client client;
}
