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

		static eastl::hash_map<int, engine::hades::Thing*, eastl::hash<int>, eastl::equal_to<int>> GetUnitMap()
		{
			return (eastl::hash_map<int, engine::hades::Thing*, eastl::hash<int>, eastl::equal_to<int>>)((uintptr_t)engine::addresses::unitmanager::map_instance - 0x8);
		}

		static int GetNumUnits()
		{
			uintptr_t begin = *(uintptr_t*)(engine::addresses::unitmanager::all_units);
			uintptr_t end = *(uintptr_t*)((uintptr_t)engine::addresses::unitmanager::all_units + 0x8);
			return (end - begin) / 0x8;
		}

		static engine::hades::Thing** GetAllUnits()
		{
			return *(engine::hades::Thing***)(engine::addresses::unitmanager::all_units);
		}
	};

}