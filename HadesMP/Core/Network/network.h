#pragma once

#include <iostream>
#include <future>
#include <ppl.h>
#include <tuple>
#include <concurrent_vector.h>

namespace core::network
{
	bool is_host = false;

	struct NetworkCallback
	{
		NetworkCallback(packets::PacketType type, std::function<void(std::string)> callback) : type(type), callback(callback) {}

		packets::PacketType type;
		std::function<void(std::string data)> callback;
	};

	struct server_client
	{
		unsigned int _uid;
		unsigned int get_uid() const { return _uid; } //MUST return globally unique value here
	};

	class Server
	{
	public:
		enetpp::trace_handler trace_handler;
		enetpp::server<server_client> server;
		std::mutex mutex;
		concurrency::concurrent_vector<std::tuple<int, std::vector<enet_uint8>>> queue; //maybe use a map lol
		concurrency::concurrent_vector<NetworkCallback> callbacks;

	public:
		std::unique_ptr<std::thread> Init();
		void Thread();
		void StartListening(const std::string& hostname, enet_uint16 port);
		void StopListening();
		void Send(int id, std::vector<enet_uint8> message);
		void Invoke();
		void OnSync(const std::string& name, std::function<void(std::string)> function);
		void Consume();
	};
	extern Server server;

	class Client
	{
	public:
		enetpp::trace_handler trace_handler;
		enetpp::client client;
		std::mutex mutex;
		concurrency::concurrent_vector<std::string> queue;
		concurrency::concurrent_vector<NetworkCallback> callbacks;

	public:
		std::unique_ptr<std::thread> Init();
		void Thread();
		void Connect(const std::string& hostname, enet_uint16 port);
		void Disconnect();
		void Send(std::string message);
		void Invoke();
		void OnSync(const std::string& name, std::function<void(std::string)> function);
		void Consume();
	};

	extern Client client;
}