#pragma once

namespace engine::hades
{
	class PlayerManager
	{
	public:
		std::vector<DWORD64> inputs; // TODO: Replace with InputHandler
		std::vector<hades::Player*> players;

	public:

		static PlayerManager* Instance()
		{
			return static_cast<PlayerManager * (__fastcall*)()>((PVOID)engine::addresses::playermanager::functions::instance)();
		}
	};
}