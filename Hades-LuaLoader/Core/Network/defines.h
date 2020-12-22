#pragma once
#include <cista/include/cista.h>

namespace core::network
{
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

	struct NetworkPlayer
	{
		NetworkClient client;
		// location
		// animation state
		// velocity
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