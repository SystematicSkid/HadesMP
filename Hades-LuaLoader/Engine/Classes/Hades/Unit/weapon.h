#pragma once

namespace engine::hades
{
	struct WeaponData : public engine::hades::GameData
	{
		BYTE mType[1];
		engine::misc::HashGuid mControl;
		engine::misc::HashGuid mAlternateControl;
		engine::misc::HashGuid mSecondaryControl;
		engine::misc::HashGuid mDisallowedControl;
		float mControlWindow;
		bool mDefaultControl;
		bool mLoseControlOnRelease;
		bool mLoseControlIfNotCharging;
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
		bool mCancelChargeOnControlRemoved;
		engine::misc::HashGuid mControlHelpTextId;
		bool mEnabled;
		engine::misc::HashGuid mPreFireAnimation;
		engine::misc::HashGuid mFireGraphic;
		engine::misc::HashGuid mFireEndGraphic;
		engine::misc::HashGuid mFailedToFireCooldownAnimation;
		float mFailedToFireCooldownDuration;
		engine::misc::HashGuid mTriggerReleaseGraphic;
		bool mAlwaysPlayTriggerReleaseAnim;
		engine::misc::HashGuid mFireFx;
		engine::misc::HashGuid mFireEndFx;
		engine::misc::HashGuid mFireFx2;
		float mFireFxOffset;
		engine::misc::HashGuid mUnblockedBlinkFx;
		engine::misc::HashGuid mClipStartFx;
		engine::misc::SoundCue mFireSound;
		bool mPlayFireSoundForPausedProjectiles;
		bool mStopFireSoundOnClipEmpty;
		float mInitialCooldown;
		float mCooldown;
		float mCooldownMin;
		float mCooldownMax;
		float mBelowMinChargeCooldown;
		int mClipSize;
		float mClipRegenInterval;
		bool mFullClipRegen;
		bool mClipRegenOnRelease;
		float mClipRegenIntervalReleased;
		float mReloadTime;
		float mReloadTimeMin;
		float mReloadTimeMax;
		bool mReloadOnRelease;
		bool mReloadOnCreation;
		bool mReloadOnTouchdown;
		engine::misc::HashGuid mReloadGraphic;
		engine::misc::HashGuid mReloadGraphicFinish;
		engine::misc::HashGuid mReloadFx;
		bool mUseReloadGraphic;
		engine::misc::SoundCue mReloadSound;
		engine::misc::HashGuid mProjectile;
		int mNumProjectiles;
		bool mRefreshProjectileOnFire;
		float mBonusProjectileChance;
		bool mNumProjectilesIsAmmo;
		bool mTreatProjectilesAsUniqueVolleys;
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
		bool mBarrelRequiresLoS;
		float mFireOffsetX;
		float mFireOffsetY;
		float mInvulnerableImpactCheckOffset;
		engine::misc::HashGuid mFireFromObstacle;
		engine::misc::HashGuid mFireToObstacle;
		bool mDestroyTargetedObstacle;
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
		bool mRootOwnerWhileFiring;
		bool mBlockMoveInput;
		float mRootOwnerWhileFiringDuration;
		float mDisableAttackForDuration;
		float mArsenalCooldown;
		float mTriggerReleaseDisableDuration;
		bool mIgnoreForceCooldown;
		float mCooldownWeaponSlotThree;
		bool mCanTargetSelf;
		bool mUseTargetAngle;
		bool mRestrictTargetAngleToAnim;
		float mSpeedMultiplier;
		int mStartingAmmo;
		int mMaxAmmo;
		engine::misc::HashGuid mShareAmmoWeapon;
		bool mSpendAmmoPerProjectile;
		bool mCancelAttachedProjectiles;
		bool mTriggersStoredAmmoVulnerability;
		bool mFireOnRelease;
		bool mForceMaxChargeRelease;
		bool mCancelChargeOnRelease;
		bool mForceReleaseOnFail;
		bool mForceReleaseOnSwap;
		bool mForceReleaseOnFire;
		engine::misc::HashGuid mForceReleaseWeaponOnFire;
		engine::misc::HashGuid mClearWeaponFireRequestOnFire;
		bool mForceReleaseAllOnFire;
		bool mAllowExternalForceRelease;
		bool mAllowExternalForceReleaseIfMinCharged;
		bool mAllowExternalForceReleaseIfAnyCharged;
		bool mCancelFireOnExternalForceRelease;
		engine::misc::HashGuid mSwapOnRelease;
		engine::misc::HashGuid mSwapOnFire;
		engine::misc::HashGuid mAddOnFire;
		engine::misc::HashGuid mSwapOnReloadFinish;
		float mMinChargeToFire;
		bool mTriggerTapIgnoresCooldown;
		bool mFullyAutomatic;
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
		bool mOnlyChargeOnce;
		float mWeaponRange;
		float mMinRange;
		bool mAutoLock;
		bool mAutoLockWithMouse;
		bool mAutoLockUseMouseOver;
		float mAutoLockRange;
		float mAutoLockArcDistance;
		bool mCanAutoLockUnits;
		bool mCanAutoLockInteractable;
		bool mCanAutoLockFriends;
		engine::misc::HashGuid mAutoLockIgnore;
		bool mManualAiming;
		float mManualAimingInitialOffset;
		bool mManualAimingRequireValidLocation;
		bool mShowFreeAimLine;
		float mAimArcInterval;
		engine::misc::HashGuid mAimLineAnimation;
		engine::misc::HashGuid mTargetReticleAnimation;
		float mTargetReticleOffset;
		bool mRotateTargetReticle;
		float mMouseApproachRange;
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
		float mMinFireZ;
		float mMaxFireZ;
		float mMaxSelfVelocityZ;
		float mAllEffectsProcChance;
		bool mSilentImpact;
		bool mSilentImpactAfterFirst;
		float mObstacleDamageMultiplier;
		float mDamageMultiplier;
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
		engine::misc::HashGuid mCancelledRamWeapon;
		float mBlinkDuration;
		float mBlinkEaseIn;
		float mBlinkEaseOut;
		float mBlinkMaxRange;
		bool mBlinkDetonateAtOrigin;
		bool mBlinkDetonateAtEndpoint;
		float mBlinkDetonateAtEndpointDelay;
		float mBlinkDetonateOnInterval;
		float mBlinkNumGhosts;
		engine::misc::HashGuid mPostBlinkAnim;
		bool mCanUseKeyboardAim;
		engine::misc::HashGuid mBlinkGhostAnimation;
		float mBlinkCameraLerpTime;
		float mSpawnOnDetonateInterval;
		float mRamMinVelocity;
		bool mRamReappliesVelocity;
		float mRamTotalDistance;
		float mRamEndVelocity;
		float mRamDistanceInterval;
		float mFlashDuration;
		bool mOwnerFaceProjectile;
		float mCheckMovementReactionDistance;
		bool mCheckDanger;
		bool mDestroySpawnsOnUnequip;
		std::vector<DWORD64> effects;
	};

	struct WeaponArsenal
	{
		class Unit* pOwner;
		std::vector<class Weapon*> mWeapons;
		std::vector<class Weapon*> mControllableWeapons;
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

		std::vector<Weapon*> GetChildren(WeaponArsenal arsenal)
		{
			std::vector<Weapon*> result;
			for (auto weapon : arsenal.mControllableWeapons)
			{
				if(weapon == this)
					continue;
				if (weapon->pGainedControlFrom == this)
					result.push_back(weapon);
			}
			return result;
		}
	};
}