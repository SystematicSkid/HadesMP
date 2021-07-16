#pragma once

namespace engine::hades
{
	struct WeaponData : public engine::hades::GameData
	{
		std::vector<PVOID> mEffects;
		engine::hades::WeaponType mType;
		engine::misc::HashGuid mControl;
		engine::misc::HashGuid mAlternateControl;
		engine::misc::HashGuid mSecondaryControl;
		engine::misc::HashGuid mDisallowedControl;
		float mControlWindow;
		engine::misc::HashGuid mRemoveControlOnCharge;
		engine::misc::HashGuid mRemoveControlOnCharge2;
		engine::misc::HashGuid mRemoveControlOnCharge3;
		engine::misc::HashGuid mRemoveControlOnCharge4;
		engine::misc::HashGuid mRemoveControlOnCharge5;
		engine::misc::HashGuid mRemoveControlOnFire;
		engine::misc::HashGuid mRemoveControlOnFire2;
		engine::misc::HashGuid mRemoveControlOnFire3;
		engine::misc::HashGuid mRemoveControlOnFire4;
		engine::misc::HashGuid mAddControlOnFire;
		engine::misc::HashGuid mAddControlOnFire2;
		engine::misc::HashGuid mAddControlOnFire3;
		engine::misc::HashGuid mAddControlOnChargeCancel;
		engine::misc::HashGuid mAddControlOnChargeCancel2;
		engine::misc::HashGuid mRemoveAllControlSwaps;
		engine::misc::HashGuid mControlHelpTextId;
		engine::misc::HashGuid mPreFireAnimation;
		engine::misc::HashGuid mFireGraphic;
		engine::misc::HashGuid mFireEndGraphic;
		engine::misc::HashGuid mFailedToFireCooldownAnimation;
		float mFailedToFireCooldownDuration;
		engine::misc::HashGuid mTriggerReleaseGraphic;
		engine::misc::HashGuid mFireFx;
		engine::misc::HashGuid mFireEndFx;
		engine::misc::HashGuid mFireFx2;
		float mFireFxOffset;
		engine::misc::HashGuid mUnblockedBlinkFx;
		engine::misc::HashGuid mClipStartFx;
		engine::misc::SoundCue mFireSound;
		float mInitialCooldown;
		float mCooldown;
		float mCooldownMin;
		float mCooldownMax;
		float mBelowMinChargeCooldown;
		int mClipSize;
		float mClipRegenInterval;
		float mClipRegenIntervalReleased;
		float mReloadTime;
		float mReloadTimeMin;
		float mReloadTimeMax;
		engine::misc::HashGuid mReloadGraphic;
		engine::misc::HashGuid mReloadGraphicFinish;
		engine::misc::HashGuid mReloadFx;
		engine::misc::SoundCue mReloadSound;
		engine::misc::HashGuid mProjectile;
		int mNumProjectiles;
		float mBonusProjectileChance;
		float mProjectileOffsetStartDistance;
		float mProjectileOffset;
		float mProjectileAngleStartOffset;
		int mProjectileAngleResetCount;
		float mProjectileAngleOffset;
		float mProjectileAngleOffsetMin;
		float mProjectileAngleOffsetMax;
		float mProjectileAngleOffsetScaleY;
		float mProjectileSpacing;
		float mProjectileInterval;
		BYTE mProjectileOffsetStart[1];
		float mAdditionalWaveDamageMultiplier;
		int mNumProjectileWaves;
		float mProjectileWaveInterval;
		float mActiveProjectileCap;
		float mFiringArc;
		float mSpread;
		float mScatter;
		float mScatterSubside;
		float mScatterCap;
		float mBarrelLength;
		float mFireOffsetX;
		float mFireOffsetY;
		float mInvulnerableImpactCheckOffset;
		engine::misc::HashGuid mFireFromObstacle;
		engine::misc::HashGuid mFireToObstacle;
		float mChargeTime;
		int mChargeTimeFrames;
		engine::misc::HashGuid mStoreChargeOnSwapFrom;
		engine::misc::HashGuid mChargeStartAnimation;
		engine::misc::HashGuid mChargeFx;
		engine::misc::HashGuid mChargeStartFx;
		engine::misc::HashGuid mMinChargeStartFx;
		engine::misc::SoundCue mChargeSound;
		engine::misc::SoundCue mChargeFullSound;
		float mChargeSoundFadeTime;
		__declspec(align(8)) engine::misc::SoundCue mOnWeaponChargingSound;
		float mOnWeaponChargingSoundChance;
		engine::misc::HashGuid mChargeCancelGraphic;
		engine::misc::HashGuid mChargeFinishFx;
		__declspec(align(8)) engine::misc::SoundCue mChargeFinishSound;
		float mChargeDamageMultiplier;
		float mChargeRangeMultiplier;
		float mChargeSpeedMultiplier;
		float mChargeRadiusMultiplier;
		float mChargeScaleMultiplier;
		float mChargeVelocityMultiplier;
		float mChargeDetachVelocityMultiplier;
		float mPerfectChargeWindowDuration;
		float mPerfectChargeDamageMultiplier;
		float mPerfectChargeRangeMultiplier;
		float mPerfectChargeSpeedMultiplier;
		float mPerfectChargeRadiusMultiplier;
		float mPerfectChargeVelocityMultiplier;
		float mPerfectChargeDetachVelocityMultiplier;
		float mFullChargeSpread;
		float mOutOfRangeSelfVelocity;
		float mSelfVelocity;
		float mSelfVelocityCap;
		float mSelfUpwardVelocity;
		float mSelfUpwardVelocityCap;
		float mSelfUpwardVelocityOnGround;
		float mDetachVelocity;
		float mDetachUpwardVelocity;
		float mDuration;
		float mDisableOwnerForDuration;
		int mDisableOwnerForDurationFrames;
		float mRootOwnerWhileFiringDuration;
		float mDisableAttackForDuration;
		float mArsenalCooldown;
		float mTriggerReleaseDisableDuration;
		float mCooldownWeaponSlotThree;
		float mSpeedMultiplier;
		int mStartingAmmo;
		int mMaxAmmo;
		engine::misc::HashGuid mShareAmmoWeapon;
		engine::misc::HashGuid mForceReleaseWeaponOnFire;
		engine::misc::HashGuid mClearWeaponFireRequestOnFire;
		engine::misc::HashGuid mSwapOnRelease;
		engine::misc::HashGuid mSwapOnFire;
		engine::misc::HashGuid mAddOnFire;
		engine::misc::HashGuid mSwapOnReloadFinish;
		float mMinChargeToFire;
		engine::misc::HashGuid mPostFireWeapon;
		float mPostFireWeaponWindow;
		engine::misc::HashGuid mCollisionWeapon;
		engine::misc::HashGuid mMovingWeapon;
		float mMovingWeaponThreshold;
		engine::misc::HashGuid mCooldownWeapon;
		float mChargeRumbleLeft;
		float mChargeRumbleRight;
		float mScreenShakeSpeed;
		float mBarrelArcLimit;
		float mWeaponRange;
		float mMinRange;
		float mAutoLockRange;
		float mAutoLockArcDistance;
		engine::misc::HashGuid mAutoLockIgnore;
		float mManualAimingInitialOffset;
		float mAimArcInterval;
		engine::misc::HashGuid mAimLineAnimation;
		engine::misc::HashGuid mTargetReticleAnimation;
		float mTargetReticleOffset;
		float mMouseApproachRange;
		float mMinFireZ;
		float mMaxFireZ;
		float mMaxSelfVelocityZ;
		float mAllEffectsProcChance;
		float mObstacleDamageMultiplier;
		float mDamageMultiplier;
		engine::misc::HashGuid mCancelledRamWeapon;
		float mBlinkDuration;
		float mBlinkEaseIn;
		float mBlinkEaseOut;
		float mBlinkMaxRange;
		float mBlinkDetonateAtEndpointDelay;
		float mBlinkDetonateOnInterval;
		float mBlinkNumGhosts;
		engine::misc::HashGuid mPostBlinkAnim;
		engine::misc::HashGuid mBlinkGhostAnimation;
		float mBlinkCameraLerpTime;
		float mSpawnOnDetonateInterval;
		float mRamMinVelocity;
		float mRamTotalDistance;
		float mRamEndVelocity;
		float mRamDistanceInterval;
		float mFlashDuration;
		float mCheckMovementReactionDistance;
		bool mDefaultControl;
		bool mLoseControlOnRelease;
		bool mLoseControlIfNotCharging;
		bool mCancelChargeOnControlRemoved;
		bool mEnabled;
		bool mAlwaysPlayTriggerReleaseAnim;
		bool mPlayFireSoundForPausedProjectiles;
		bool mStopFireSoundOnClipEmpty;
		bool mFullClipRegen;
		bool mClipRegenOnRelease;
		bool mReloadOnRelease;
		bool mReloadOnCreation;
		bool mReloadOnTouchdown;
		bool mUseReloadGraphic;
		bool mRefreshProjectileOnFire;
		bool mNumProjectilesIsAmmo;
		bool mTreatProjectilesAsUniqueVolleys;
		bool mBarrelRequiresLoS;
		bool mDestroyTargetedObstacle;
		bool mRootOwnerWhileFiring;
		bool mBlockMoveInput;
		bool mIgnoreForceCooldown;
		bool mCanTargetSelf;
		bool mUseTargetAngle;
		bool mRestrictTargetAngleToAnim;
		bool mSpendAmmoPerProjectile;
		bool mCancelAttachedProjectiles;
		bool mTriggersStoredAmmoVulnerability;
		bool mFireOnRelease;
		bool mForceMaxChargeRelease;
		bool mCancelChargeOnRelease;
		bool mForceReleaseOnFail;
		bool mForceReleaseOnSwap;
		bool mForceReleaseOnFire;
		bool mForceReleaseAllOnFire;
		bool mAllowExternalForceRelease;
		bool mAllowExternalForceReleaseIfMinCharged;
		bool mAllowExternalForceReleaseIfAnyCharged;
		bool mCancelFireOnExternalForceRelease;
		bool mTriggerTapIgnoresCooldown;
		bool mFullyAutomatic;
		bool mOnlyChargeOnce;
		bool mAutoLock;
		bool mAutoLockWithMouse;
		bool mAutoLockUseMouseOver;
		bool mCanAutoLockUnits;
		bool mCanAutoLockInteractable;
		bool mCanAutoLockFriends;
		bool mManualAiming;
		bool mManualAimingRequireValidLocation;
		bool mShowFreeAimLine;
		bool mRotateTargetReticle;
		bool mReloadOnEquip;
		bool mLockTrigger;
		bool mLockTriggerForClip;
		bool mLockTriggerForCharge;
		bool mLockTriggerTransferFromOnSwap;
		bool mRequestTriggerLockDuringReload;
		bool mAcceptTriggerLockRequests;
		bool mIgnoreTriggerLockDuringCooldown;
		bool mClearFireRequestOnSwap;
		bool mClearFireRequestOnChargeCancel;
		bool mAllowMultiFireRequest;
		bool mPriorityFireRequest;
		bool mSilentImpact;
		bool mSilentImpactAfterFirst;
		bool mRandomFireAngle;
		bool mFireAtAttackTarget;
		bool mCancelMovement;
		bool mChargeCancelMovement;
		bool mStrafe;
		bool mSetCompleteAngleOnCharge;
		bool mSetCompleteAngleOnFire;
		bool mSetTargetAngleOnRequest;
		bool mCanFireWhileDisarmed;
		bool mCanFireWithAttached;
		bool mCanFireWithoutAttached;
		bool mCanPullTriggerWithAutoLock;
		bool mCanPullTriggerWithoutMoving;
		bool mIgnoreOwnerAttackDisabled;
		bool mCanCancelDisables;
		bool mCanCancelOwnDisables;
		bool mCanCancelRotateDisables;
		bool mCancelRamWeaponOnCharge;
		bool mCancelRamWeaponOnFire;
		bool mBlinkDetonateAtOrigin;
		bool mBlinkDetonateAtEndpoint;
		bool mCanUseKeyboardAim;
		bool mRamReappliesVelocity;
		bool mOwnerFaceProjectile;
		bool mCheckDanger;
		bool mDestroySpawnsOnUnequip;
	};

	class Weapon : public engine::Rtti
	{
	public:
		bool mReloadPaused;
		bool mRequireControlRelease;
		bool mWasLastFirePerfectCharge;
		bool mDisableOnNextFire;
		bool mCharging;
		float mImpactCueStopTime;
		float mLastFireTime;
		float mControlTime;
		float mLastSpawnOnDetonateTime;
		float mCooldownTimeRemaining;
		float mChargeTimeRemaining;
		int mOwnerId;
		int mAmmo;
		int mVolley;
		int mImpactCue;
		int mOverrideFireRequestTarget;
		DWORD64* pControl;
		DWORD64* pAlternateControl;
		DWORD64* pSecondaryControl;
		DWORD64* pDisallowedControl;
		DWORD64* pProjectileData;
		DWORD64* pSpawnedProjectileData;
		DWORD64* pDelayedProjectile;
		WeaponData* pData;
		Weapon* pGainedControlFrom;
		std::vector<int> mSpawnedIds;
		std::optional<PVOID> mCurrentFireRequest;
		std::optional<PVOID> mPrevFireRequest;
		std::basic_string<char> mOverrideWeaponFire;
		bool mValidProjectileData;
		bool mValidSpawnedProjectileData;
		bool mPerfectChargeLocked;
		bool mIsDisposed;
		float mNextClipRegenTime;
		float mCritBonusChance;
		float mAdditiveDamageMultiplier;
		float mReloadTimeRemaining;
		float mScatter;
		float mBlastRadiusModifier;
		int mCritBonusRemaining;
		int mClipShotsRemaining;
		int mAdditiveDamageBonusRemaining;
		int mFireSound;
		// TODO: Finish this!

	public:
		// Methods
		bool RequestFire(float angle, D3DXVECTOR2 target_location, engine::hades::Thing* target)
		{
			return static_cast<bool(__fastcall*)(Weapon*, float, D3DXVECTOR2, engine::hades::Thing*)>((PVOID)engine::addresses::weapon::functions::request_fire)
				(this, angle, target_location, target);
		}

		// Use Hash ToString!
		std::string ToString()
		{
			return static_cast<std::string(__fastcall*)(Weapon*)>((PVOID)engine::addresses::weapon::functions::tostring)(this);
		}

		/*std::vector<Weapon*> GetChildren(WeaponArsenal arsenal)
		{
			std::vector<Weapon*> result;
			for (auto weapon : arsenal.mControllableWeapons)
			{
				if (weapon == this)
					continue;
				if (weapon->pGainedControlFrom == this)
					result.push_back(weapon);
			}
			return result;
		}*/

		bool CanFire()
		{
			return static_cast<bool(__fastcall*)(Weapon*)>((PVOID)engine::addresses::weapon::functions::can_fire)(this);
		}


		void PostFire()
		{
			return static_cast<void(__fastcall*)(Weapon*)>((PVOID)engine::addresses::weapon::functions::post_fire)(this);
		}

		bool MustReleaseTrigger()
		{
			return reinterpret_cast<bool(__fastcall*)(Weapon*)>(engine::addresses::weapon::functions::must_release_trigger)(this);
		}

		bool HasAmmo()
		{
			return reinterpret_cast<bool(__fastcall*)(Weapon*)>(engine::addresses::weapon::functions::has_ammo)(this);
		}

		bool IsReadyToFire()
		{
			if	(!this->pData->mEnabled || this->MustReleaseTrigger() 
				//|| !this->HasAmmo() && !this->pData->mFailedToFireCooldownAnimation.id
				//|| this->pData->mClipSize && !this->mClipShotsRemaining
				//|| this->pData->mIgnoreTriggerLockDuringCooldown && this->mCooldownTimeRemaining > 0.f)
				)
			{
				return false;
			}
			return true;
		}

		void SetEffectProperty(const char* effect_name, const char* property_name, engine::misc::any* val, engine::Reflection_ValueChangeType change_type)
		{
			return reinterpret_cast<void(__fastcall*)(Weapon*, const char*, const char*, engine::misc::any*, engine::Reflection_ValueChangeType)>(engine::addresses::weapon::functions::set_effect_property)
				(this, effect_name, property_name, val, change_type);
		}

		void SetProjectileProperty(const char* name, engine::misc::any* val, engine::Reflection_ValueChangeType change_type)
		{
			return reinterpret_cast<void(__fastcall*)(Weapon*, const char*, engine::misc::any*, engine::Reflection_ValueChangeType)>(engine::addresses::weapon::functions::set_projectile_property)
				(this, name, val, change_type);
		}

		static Weapon* Create(engine::misc::HashGuid name, engine::hades::Unit* owner)
		{
			return static_cast<Weapon * (__fastcall*)(engine::misc::HashGuid, engine::hades::Unit*)>((PVOID)engine::addresses::weapon::functions::create_hash)
				(name, owner);
		}

		static Weapon* Create(WeaponData* data, engine::hades::Unit* owner)
		{
			return static_cast<Weapon * (__fastcall*)(WeaponData*, engine::hades::Unit*)>((PVOID)engine::addresses::weapon::functions::create_data)
				(data, owner);
		}
	};

	struct WeaponArsenal
	{
		class Unit* pOwner;
		std::vector<class Weapon*> mWeapons;
		std::vector<class Weapon*> mControllableWeapons;

		Weapon* GetWeapon(engine::misc::HashGuid id)
		{
			for (auto weapon : mWeapons)
			{
				if (weapon->pData->name.id == id.id)
					return weapon;
			}
			return nullptr;
		}

		std::vector<int> GetWeapons()
		{
			std::vector<int> ret;
			for (auto weapon : mWeapons)
				if(weapon && weapon->pData)
					ret.push_back(weapon->pData->name.id);
			return ret;
		}

		void DumpWeapons()
		{
			printf("[=] Arsenal Weapons\n");
			for (auto weapon : mWeapons)
			{
				printf("\t%s\n", weapon->pData->name.ToString());
			}
			printf("[=] Controllable Weapons\n");
			for (auto weapon : mControllableWeapons)
			{
				printf("\t%s\n", weapon->pData->name.ToString());
			}
		}

		void AddWeaponControl(Weapon* weapon)
		{
			return static_cast<void(__fastcall*)(WeaponArsenal*, Weapon*)>((PVOID)engine::addresses::weaponarsenal::functions::add_weapon_control)
				(this, weapon);
		}

		void AddWeapon(Weapon* weapon)
		{
			return static_cast<void(__fastcall*)(WeaponArsenal*, Weapon*, bool)>((PVOID)engine::addresses::weaponarsenal::functions::add_weapon)
				(this, weapon, false /* bundled */);
		}

		// Custom
		void AddWeapon(engine::misc::HashGuid hash)
		{
			Weapon* weapon = Weapon::Create(hash, pOwner);
			if (weapon)
				this->AddWeapon(weapon);
		}
	};
}