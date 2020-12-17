#pragma once

namespace engine::hades
{
	class UnitManager
	{
	public:
		static engine::hades::Thing* CreatePlayerUnit(engine::hades::UnitData* data, D3DXVECTOR2 location, engine::hades::MapThing* map_thing, bool do_activate_presentation, bool needs_lua)
		{
			return static_cast<engine::hades::Thing * (__fastcall*)(engine::hades::UnitData*, D3DXVECTOR2, engine::hades::MapThing*, bool, bool)>
				((PVOID)engine::addresses::unitmanager::functions::create_player_unit)(data, location, map_thing, do_activate_presentation, needs_lua);
		}
	};
}