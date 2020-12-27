#pragma once

#include <iostream>
#include <future>
#include <ppl.h>
#include <concurrent_vector.h>

#include "../3rdparty/json/json.h"
#include "../3rdparty/json/fifo_map.h"

#include "./defines.h"
#include "./server.h"
#include "./client.h"

namespace core::network
{
	class NetworkPlayer
	{
	public:
		int uuid;
		engine::misc::Color outline_color;
		engine::hades::Thing* character;
		std::string map;

		/* Unit Specific */
		D3DXVECTOR2 network_position;
		std::vector<int> weapons;

	public:
		NetworkPlayer() {}
		NetworkPlayer(int uuid) : uuid(uuid)
		{
			this->outline_color = engine::misc::Color(255, 0, 0);
		}
	};

	std::vector<NetworkPlayer*> network_players;

	NetworkPlayer* GetPlayer(int uuid)
	{
		for (auto p : network_players)
		{
			printf("UID: %i\n", p->uuid);
			if (p->uuid = uuid)
				return p;
		}
		return nullptr;
	}
}