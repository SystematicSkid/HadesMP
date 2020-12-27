#pragma once
#include <cista/include/cista.h>

namespace core::network
{

	template<class K, class V, class dummy_compare, class A>
	using my_workaround_fifo_map = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;

	using json = nlohmann::basic_json<my_workaround_fifo_map>;

	struct NetworkCallback
	{
		NetworkCallback(std::string name, std::function<void(json)> callback) : name(name), callback(callback) {}

		std::string name;
		std::function<void(json)> callback;
	};

	struct server_client
	{
		unsigned int _uid;
		unsigned int get_uid() const { return _uid; } //MUST return globally unique value here
	};

	enum class ClientState
	{
		Connected,
		Connecting,
		Preauth,
		Authorized,
		Disconnected,
		None
	};

	enum class PlayerState
	{
		Alive,
		Dead,
		None
	};

	struct NetworkClient
	{
		int guid;
		char name[32];
	};


	namespace packet
	{
		struct anime
		{
			struct position
			{
				float x;
				float y;
			} pos;

			cista::raw::string msg;
		};

		struct ServerUpdateOnTick
		{
			int time;
			int tick;
			int player_count;
			// player list
			bool player_update = true;
		};

		struct PlayerUpdate
		{
			// in response to ServerUpdateOnTick::player_update
		};
	}
}