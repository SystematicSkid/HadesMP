#pragma once

namespace core::network
{

	unsigned int next_uid = 1;
	inline auto init_client_func = [&](server_client& client, const char* ip)
	{
		client._uid = next_uid;
		next_uid++;
	};

	void serverthreadfunc()
	{
		server.Thread();
	}

	std::unique_ptr<std::thread> Server::Init()
	{
		return std::make_unique<std::thread>(&serverthreadfunc);
	}

	void Server::Thread()
	{
		enetpp::global_state::get().initialize();

		trace_handler = [&](const std::string& msg)
		{
			std::lock_guard<std::mutex> lock(mutex);
			printf("server: %s\n", msg.c_str());
		};

		server.set_trace_handler(trace_handler);

		StartListening("localhost", 27017);

		Invoke();

		std::terminate();
	}

	void Server::StartListening(const std::string& hostname, enet_uint16 port)
	{
		server.start_listening(enetpp::server_listen_params<server_client>()
			.set_channel_count(1)
			.set_max_client_count(0xFFF)
			.set_incoming_bandwidth(0)
			.set_outgoing_bandwidth(0)
			.set_listen_port(port)
			.set_initialize_client_function(init_client_func));
	}

	void Server::StopListening()
	{
		//connected = false;

		server.stop_listening();
	}

	void Server::Send(int id, std::vector<enet_uint8> message)
	{
		queue.push_back(std::tuple<int, std::vector<enet_uint8>>(id, message));
	}

	void Server::Consume()
	{
		static auto on_connected = [this](server_client& client)
		{
			trace_handler("on_connected");
			network::is_host = true;
			//for (auto c : server.get_connected_clients())
				//Send(c->get_uid(), "JoinMSGPacket", json{std::string(std::to_string(client.get_uid()) + " has joined.")});

			//connected = true;
		};

		static auto on_disconnected = [this](unsigned int client_uid)
		{
			trace_handler("on_disconnected");
		};

		static auto on_data_received = [this](server_client& client, const enet_uint8* data, size_t data_size)
		{
			try
			{
				auto handle = msgpack::unpack((char*)data, data_size);
				auto msg = handle.get();

				auto base_packet = msg.as<packets::Packet>();

				//printf("[Server] Received packet: %i\n", base_packet.type);

				std::vector<enet_uint8> send_data;
				for (int i = 0; i < data_size; i++)
					send_data.push_back(data[i]);

				/* Relay back to clients */
				for (auto& c : this->server.get_connected_clients())
				{
					if (c->get_uid() == client.get_uid())
						continue;
					this->Send(c->get_uid(), send_data);
				}

				/*std::tuple<std::string, std::string> prototype;
				auto prelude = msg.convert(prototype);

				std::string callback_name = std::get<0>(prelude);
				std::string data = std::get<1>(prelude);

				for (auto& callback : callbacks)
				{
					if (callback.name == callback_name)
						callback.callback(data);
				}*/
			}
			catch (std::exception& e)
			{
				printf("Server error\n");
				MessageBoxA(NULL, e.what(), e.what(), NULL);
			}
		};

		server.consume_events(on_connected, on_disconnected, on_data_received);
	}

	// on sync will wait til it receives some sort of response (blocking)
	void Server::OnSync(const std::string& name, std::function<void(std::string)> function)
	{
		/*std::promise<json> promise;

		callbacks.push_back(NetworkCallback(name, [&](json message) { promise.set_value(message); }));

		std::future<json> future = promise.get_future();

		function(future.get());*/
	}

	void Server::Invoke()
	{
		while (server.is_listening())
		{
			if (!server.get_connected_clients().empty())
			{
				for (auto& msg : queue)
				{
					int id = std::get<0>(msg);
					std::vector<enet_uint8> packet = std::get<1>(msg);
					if (!packet.empty())
						server.send_packet_to(id, 0, reinterpret_cast<const enet_uint8*>(packet.data()), packet.size(), ENET_PACKET_FLAG_RELIABLE);
				}

				//queue = concurrency::concurrent_vector<json>{};
				queue.clear();
			}

			Consume();

		}
	}
}