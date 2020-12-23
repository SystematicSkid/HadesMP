#pragma once

namespace engine::hades
{
	struct IThingComponent
	{
		virtual void Reset() = 0;

		bool is_enabled;
		class Thing* owner;
		std::string name;
	};

	class BodyComponent : public IThingComponent
	{
		std::vector<PVOID> active_transitions; // BodyComponentTransitionHelper
		float mScale;
		float mSquashStretchScale;
		float mSquashStretchAngle;
		float mAngle;
		float mAmbient;
		bool mDirty;
		bool mEverAdjusted;
		D3DXVECTOR2 scale_vector;
		BYTE mMeshType[1];
		BYTE mFlips[1];
		engine::misc::Color mBaseColor;
		D3DXVECTOR3 hsv;
	};

	class LifeComponent : IThingComponent
	{
	public:
		bool mTargetable;
		bool mIsInvulnerable;
		bool mIsPlayerControlled;
		bool mIsObstacle;
		int mActiveHitFxCount;
		int mConsecutiveHits;
		float mInvulnerableCoverage;
		float mInvulnerableCoverageOffset;
		float mDodgeChance;
		float mArmorCoverage;
		float mCollisionDamageMultiplier;
		float mDamageTakenMultiplier;
		float mConsecutiveHitWindow;
		float mActiveHitFxCooldown;
		float mReflection;
		class LifeData* pData;
		Animation* pInvulnerableFx;
	};

	struct PhysicsComponent : IThingComponent
	{
		D3DXVECTOR2 mZero;
		const float mSimulationTick;
		bool mBeingVacuumed;
		bool mIgnoreGravity;
		bool mLastForceIsSelfApplied;
		int mImpulsingCueId;
		float mGravity;
		float mFootingLostTime;
		float mUpwardVelocity;
		float mSpinVelocity;
		float mBounceMultiplier;
		float mTetherDistance;
		float mTetherElasticity;
		float mTetherRetractSpeed;
		float mTetherTrackZRatio;
		D3DXVECTOR2 mMoveShift;
		D3DXVECTOR2 mVelocity;
		D3DXVECTOR2 mAcceleration;
		std::vector<int> mTetheredTo;
		std::vector<int> mTetheredFrom;
	};

	struct VacuumComponent : IThingComponent
	{
		float mStrength;
		float mDistance;
		float mMinZ;
		float mMaxZ;
		float mDisableTimeOut;
		int mOwnerId;
		std::vector<Thing*> mTargets;
	};

	struct __declspec(align(8)) MagnetismComponent : IThingComponent
	{
		bool mTrailing;
		bool mRequiresUsable;
		int mTargetId;
		float mMinZ;
		float mMaxZ;
		float mTrailDistance;
	};

	struct __declspec(align(8)) TranslateComponent : IThingComponent
	{
		bool mUsePhysics;
		float mSpeed;
		float mRemainingDistance;
		D3DXVECTOR2 mDirection;
	};

	struct PlayerNearbyComponent : IThingComponent
	{
		bool mCheckAmbientSoundDistance;
		bool mPlayedAmbientSound;
		float mAmbientSoundDistanceSq;
	};

	struct __declspec(align(4)) GUIFlasher
	{
		struct
		{
			void(__fastcall* Defaults)(GUIFlasher*);
		}*vtbl;
		unsigned int mExpireAmount;
		bool mInitialized;
		bool mExpireAfterCycle;
		bool mPrevAscending;
		bool mAscending;
		float mModulationRate;
		float mMin;
		float mMax;
		float mPrevValue;
		float mExpireTime;
		float mValueOverride;
		float mValueOverrideTarget;
		float mXValue;
		engine::misc::Color mColor;
		BYTE mType[1];
	};

	/* 1586 */
	struct __declspec(align(8)) FlashComponent : IThingComponent
	{
		GUIFlasher mFlasher;
		engine::misc::Color mPermaFlashColor;
	};

	struct OffsetHelper
	{
		bool mLockedAngle;
		bool mInitialized;
		bool mEaseOut;
		bool mAllowPickAngle;
		float mMaxDistanceSqrd;
		float mTimeRemaining;
		float mMaxDistance;
		float mSpeed;
		float mSpeedAcceleration;
		float mDistanceAcceleration;
		float mAngle;
		float mEaseOutSpeed;
		D3DXVECTOR2 mLastEndPoint;
		D3DXVECTOR2 mTargetOffset;
		D3DXVECTOR2 mOffset;
	};

	struct ShakeComponent : IThingComponent
	{
		std::optional<OffsetHelper> mShakeOffset;
	};
}