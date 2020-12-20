#pragma once
#include <unordered_set>

namespace engine::hades
{

	

	struct UnitStatus
	{
		bool mDisableMoveCancelable;
		bool mDisableRotateCancelable;
		bool mDisableAttackCancelable;
		bool mUpdatingEffects;
		float mDisableMoveTime;
		float mDisableAttackTime;
		float mDisableRotateTime;
		float mAllegianceTime;
		float mBlockEffectDuration;
		engine::misc::HashGuid mBlockEffectId;
		__declspec(align(8)) std::unordered_set<DWORD64, std::hash<DWORD64>, std::equal_to<DWORD64>> mToAdd;
		std::unordered_set<DWORD64, std::hash<DWORD64>, std::equal_to<DWORD64>> mToDelete;
		std::vector<DWORD64> mEffects;
		std::vector<engine::misc::HashGuid> mImmuneEffects;
		class Unit* pOwner;
		class Player* pOriginalAllegiance;
		bool mIsTraitor;
		bool mBlockAll;
		bool padding0[6];

		// Methods
		bool CanAttack(bool can_cancel_disables)
		{
			return static_cast<bool(__fastcall*)(UnitStatus*, bool)>((PVOID)engine::addresses::unitstatus::functions::can_attack)(this, can_cancel_disables);
		}

		void SetAsAlly()
		{
			return static_cast<void(__fastcall*)(UnitStatus*)>((PVOID)engine::addresses::unitstatus::functions::set_as_ally)(this);
		}
	};

	// Note: This is a placeholder, this should be replaced by ACTUAL inheritance to the `Thing` class.
	class Unit
	{
	private:
		char pad[0x3F8];
	public:
		WeaponArsenal arsenal;
		UnitStatus status;
		std::unordered_map<unsigned int, std::hash<unsigned int>, std::equal_to<unsigned int>> mPrevColliding;
		std::unordered_map<unsigned int, std::hash<unsigned int>, std::equal_to<unsigned int>> mCurrentlyColliding;
		// End

	public:
		// Methods
		void CopyArsenal(Unit* to_copy)
		{
			auto weapons_to_copy = to_copy->arsenal.mWeapons;
			std::vector<Weapon*> new_array; // Holder for all copied weapons
			for (auto weapon : weapons_to_copy)
			{
				Weapon* new_weapon = Weapon::Create(weapon->pData->name, this); // Generate new weapon pointer from data and unit type
				if (new_weapon)
					new_array.push_back(new_weapon);
				else
					printf("[ Error ] Failed to create new weapon!\n");
			}
			
			this->arsenal.mWeapons.clear();
			this->arsenal.mControllableWeapons.clear();
			for (auto weapon : new_array)
				this->arsenal.AddWeapon(weapon);

		}

		void DrawExtents()
		{
			return static_cast<void(__fastcall*)(Unit*)>((PVOID)engine::addresses::unit::functions::draw_extends)(this);
		}
	};

	
}