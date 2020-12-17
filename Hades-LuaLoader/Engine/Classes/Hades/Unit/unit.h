#pragma once
#include <unordered_set>

namespace engine::hades
{
	struct WeaponArsenal
	{
		class Unit* pOwner;
		std::vector<engine::hades::Weapon*> mWeapons;
		std::vector<engine::hades::Weapon*> mControllableWeapons;
	};

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
	};

	// Note: This is a placeholder, this should be replaced by ACTUAL inheritance to the `Thing` class.
	class Unit
	{
	private:
		char pad[0x3F8];
	public:
		WeaponArsenal arsenal;
		UnitStatus status;
	};
}