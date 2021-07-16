#pragma once

namespace engine::hades
{
	class PlayerManager
	{
	public:
		std::vector<engine::hades::InputHandler*> inputs; // TODO: Replace with InputHandler
		std::vector<hades::Player*> players;

	public:

		static PlayerManager* Instance()
		{
			return static_cast<PlayerManager * (__fastcall*)()>((PVOID)engine::addresses::playermanager::functions::instance)();
		}

		hades::Player* AddPlayer(int index)
		{
			return static_cast<hades::Player * (__fastcall*)(PlayerManager*, int)>((PVOID)engine::addresses::playermanager::functions::add_player)(this, index);
		}

		void AssignController(hades::Player* player, uint8_t index)
		{
			return reinterpret_cast<void(__fastcall*)(PlayerManager*, hades::Player*, uint8_t)>(engine::addresses::playermanager::functions::assign_controller)(this, player, index);
		}
	};
}