#pragma once
#include <unordered_set>

namespace engine::hades
{

	struct UnitAI
	{
		engine::hades::MoveAI* pActiveMoveAI;
		class Unit* pOwner;
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

		void SetAsAlly()
		{
			return static_cast<void(__fastcall*)(UnitStatus*)>((PVOID)engine::addresses::unitstatus::functions::set_as_ally)(this);
		}
	};

	class Unit : public Thing
	{
	private:
		// Virtuals
		virtual void unk12() = 0;
		virtual void unk13() = 0;
		virtual void StopMoving() = 0;
	public:
		engine::hades::Player* pPlayer;
		bool mAllowInput;
		bool mWasTraitor;
		bool mIsMoving;
		bool mIsMovingTo;
		bool mHasActivated;
		bool mAllowAnyFire;
		bool padding0;
		char mNumThingsTrailing;
		engine::hades::UnitData* pData;
		int mSpawnedBy;
		float mUnappliedSpeed;
		float mCosmeticAngle;
		float mCosmeticAngleTarget;
		float mSpeedFraction;
		float mSpeedModifier;
		float mSpeedAddition;
		float mPrevSpeedFraction;
		float mGoalAngle;
		float mDamageOutputModifier;
		D3DXVECTOR2 mTetherLocation;
		D3DXVECTOR2 mRectangleError;
		D3DXVECTOR2 mPrevLocation;
		D3DXVECTOR2 mLastMoveStartLocation;
		D3DXVECTOR2 mLastCollisionTranslation;
		engine::hades::UnitAI mAI;
		WeaponArsenal arsenal;
		UnitStatus status;
		eastl::hash_map<unsigned int, eastl::hash<unsigned int>, eastl::equal_to<unsigned int>> mPrevColliding;
		eastl::hash_map<unsigned int, eastl::hash<unsigned int>, eastl::equal_to<unsigned int>> mCurrentlyColliding;
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