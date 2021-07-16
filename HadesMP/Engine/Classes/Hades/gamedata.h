#pragma once

namespace engine::hades
{
	struct GameData
	{
		struct  
		{
			void(__fastcall* unk1)(GameData*);
		}*vtbl;
		engine::misc::HashGuid name;
		engine::misc::HashGuid organization;
		engine::misc::HashGuid inherit_from;
	};

	// Not really associated by anything but name
	class GameDataManager
	{
	public:
		// Methods
		class WeaponData* GetWeaponData(engine::misc::HashGuid name)
		{
			return static_cast<class WeaponData* (__fastcall*)(engine::misc::HashGuid)>((PVOID)engine::addresses::gamedatamanager::functions::get_weapon_data)(name);
		}

		static class UnitData* GetUnitData(engine::misc::HashGuid name)
		{
			return reinterpret_cast<class UnitData* (__fastcall*)(engine::misc::HashGuid)>(engine::addresses::gamedatamanager::functions::get_unit_data)(name);
		}
	};
}