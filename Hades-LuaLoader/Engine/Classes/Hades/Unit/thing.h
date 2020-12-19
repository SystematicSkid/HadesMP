#pragma once

namespace engine::hades
{
	

	struct __declspec(align(0x8)) ThingData
	{
		bool created;
		int pad_0x4;
		engine::misc::HashGuid graphic;
		engine::misc::HashGuid activate_graphic;
		engine::misc::HashGuid activate_fx;
		engine::misc::SoundCue ambient_sound;
		bool high_priority_sfx;
		engine::misc::HashGuid impulsed_animation;
		engine::misc::HashGuid impulsed_fx;
		float impulsed_fx_cooldown;
		engine::misc::SoundCue impulsing_sound;
		float impulsing_sound_fade_out_duration;
		engine::misc::SoundCue footstep_sound;
		engine::misc::SoundCue rotation_sound;
		float rotation_sound_threshold;
		engine::misc::HashGuid footstepfx_l;
		engine::misc::HashGuid footstepfx_r;
		float tallness;
		D3DXVECTOR2 size;
		float mRelativeExtentScale;
		float mExtentScale;
		float mSortBoundsScale;
		float mRadius;
		float mSelectionRadius;
		float mSelectionWidth;
		float mSelectionHeight;
		float mSelectionShiftY;
		float mScale;
		float mMinScale;
		float mMaxScale;
		float mStartAngle;
		float mOffsetZ;
		float mMaxZ;
		float mOriginX;
		float mOriginY;
		engine::misc::HashGuid mAttachToMarker;
		bool mInvisible;
		float mBounceMultiplier;
		float mBounceUpwardForce;
		float mBounceSideForce;
		float mGrip;
		float mDrag;
		float mGravity;
		float mTerminalVelocity;
		float mUpwardTerminalVelocity;
		float mSpinFactor;
		float mSpinDeceleration;
		bool mImmuneToVacuum;
		bool mImmuneToForce;
		float mExternalForceMultiplier;
		float mTetherOffset;
		float mRetractDistance;
		bool mRotateGeometry;
		bool mMirrorGeometry;
		bool mCreatesShadows;
		bool mStopsLight;
		bool mStopsUnits;
		bool mStopsProjectiles;
		engine::misc::Color mColor;
		float mHue;
		float mSaturation;
		float mValue;
		bool mAddColor;
		bool mCanBeOccluded;
		bool mCausesOcclusion;
		float mOcclusionWidth;
		float mOcclusionHeight;
		float mOcclusionOpacity;
		float mOcclusionInterpolateDuration;
		float mOcclusionAreaScalar;
		bool mOutlineAllAnims;
		float mOcclusionOutlineRed;
		float mOcclusionOutlineGreen;
		float mOcclusionOutlineBlue;
		float mOcclusionOutlineThickness;
		float mOcclusionOutlineOpacity;
		float mOcclusionOutlineThreshold;
		float mOcclusionOutlineDelay;
		float mOcclusionOutlineFadeSpeed;
		float mOutlineThickness;
		float mOutlineThreshold;
		float mOutlineOpacity;
		engine::misc::Color mOutlineColor;
		bool mHighlightTopAnimOnly;
		engine::misc::HashGuid mOcclusionBaseAnim;
		BYTE mSortMode[1];
		bool mDrawEmbeddedBehind;
		bool mDrawVfxOnTop;
		bool mUseBoundsForSortDrawArea;
		bool mUseScreenLocation;
		bool mAllowDrawableCache;
		engine::misc::HashGuid mAttachedAnim;
		engine::misc::SoundCue mTouchdownSound;
		engine::misc::HashGuid mOnTouchdownFxAnim;
		float mTouchdownRumbleIntensity;
		float mTouchdownRumbleDuration;
		float mTouchdownScreenShake;
		engine::misc::HashGuid mTouchdownGraphic;
		bool mDieOnTouchdown;
		engine::misc::HashGuid mLight;
		BYTE mMeshType[1];
		bool mSpeechCapable;
		bool mSpeechMetering;
		engine::misc::HashGuid mSpeechMeteringChannelName;
		engine::misc::Color mSubtitleColor;
		D3DXVECTOR2 mOffset;
		float mTimeModifierFraction;
		bool mTriggerOnSpawn;
		bool mEditorOutlineDrawBounds;
		float mAmbient;
		std::vector<D3DXVECTOR2> mPoints;
		std::vector<D3DXVECTOR2> mOccluderPoints;
		DWORD64* mInteract;
		DWORD64* mMetering;
		std::vector<DWORD64> mUsing;
	};

	class Thing : public RenderComponent
	{
	public:
		bool mVisible;
		bool mAnimFreeze;
		bool mSuppressSounds;
		bool mIsDisposed;
		int mPrevGridStart;
		int mPrevGridEnd;
		int mId;
		float mAttachOffsetZ;
		float mZLocation;
		float mTallness;
		float mTimeModifierFraction;
		float mElapsedTimeMultiplier;
		float mSpawnTime;
		D3DXVECTOR2 location;
		D3DXVECTOR2 spawn_location;
		engine::misc::IRectangle rectangle;
		D3DXVECTOR3 direction;
		ThingData* thing_data;
		BodyComponent* body;
		LifeComponent* pLife;
		PhysicsComponent* pPhysics;
		VacuumComponent* pVacuum;
		MagnetismComponent* pMagnetism;
		DWORD64 pLightOccluder;
		TranslateComponent* pTranslate;
		PlayerNearbyComponent* pPlayerNearby;
		FlashComponent* pFlash;
		ShakeComponent* pShake;

	public:
		void MoveInput(D3DXVECTOR2* location, float speed_fraction, bool strafe, float elapsed_seconds)
		{
			return static_cast<void(__fastcall*)(Thing*, D3DXVECTOR2*, float, bool, float)>((PVOID)engine::addresses::unit::functions::move_input)
				(this, location, speed_fraction, strafe, elapsed_seconds);
		}
	};

	struct __declspec(align(8)) EntityLinkedObjectThing
	{
		Thing* mObject;
		unsigned int mEntity;
	};

	struct MapThing
	{
		BYTE mDataType[1];
		engine::misc::HashGuid mName;
		D3DXVECTOR2 mLocation;
		int mId;
		std::vector<DWORD> mGroupNames;
		bool mFrozen;
		bool mSelectable;
		bool mVisible;
		bool mActive;
		bool mActivateAtRange;
		float mActivationRange;
		engine::misc::HashGuid mHelpTextId;
		engine::misc::Color mColor;
		float mAngle;
		float mScale;
		float mOffsetZ;
		float mTallness;
		float mSkewAngle;
		float mSkewScale;
		bool mFlipHorizontal;
		bool mFlipVertical;
		int mAttachToID;
		std::vector<int> mAttachedIDs;
		bool mInvert;
		bool mClutter;
		BYTE mCreatesShadows[1];
		BYTE mStopsLight[1];
		float mAmbient;
		std::string mComments;
		std::vector<D3DXVECTOR2> mPoints;
		bool mCollision;
		bool mCausesOcclusion;
		bool mIgnoreGridManager;
		bool mAllowMovementReaction;
		BYTE mDrawVfxOnTop[1];
		BYTE mUseBoundsForSortArea[1];
		float mParallaxAmount;
		float mPrevParallaxAmount;
		bool mParallaxDirty;
		int mSortIndex;
		std::vector<DWORD64*> m_gridSquares;
		float mHue;
		float mSaturation;
		float mValue;
	};
}