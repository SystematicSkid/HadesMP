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

	struct __declspec(align(8)) EntityLinkedObjectThing
	{
		class Thing* mObject;
		unsigned int mEntity;
	};

	class Thing : public RenderComponent
	{
	public:
		// Virtual functions
		virtual bool IsDone() = 0;
		virtual bool ShouldDraw() = 0;
		virtual void DrawExtents() = 0;
		virtual void UpdateAnimation(float) = 0;
		virtual void OnUpwardForce(float) = 0;
		virtual void OnTouchdown(float, float) = 0;
		virtual D3DXVECTOR2* GetDrawLocation(D3DXVECTOR2*, D3DXVECTOR2*, float) = 0;
		virtual bool IsOnCamera() = 0;
		virtual void MarkSortScoreChanged() = 0;
		virtual void ShiftZLocation(float, bool) = 0;
		virtual void SetLocation(D3DXVECTOR2*, bool) = 0;
		virtual engine::misc::HashGuid* GetName(engine::misc::HashGuid*) = 0;
		virtual void SetAngle(float) = 0;
		virtual void SetAttachOffset(D3DXVECTOR2*) = 0;
		virtual engine::misc::IRectangle* GetArea(engine::misc::IRectangle*) = 0;
		virtual void unk5() = 0;
		virtual void unk6() = 0;
		virtual void unk7() = 0;
		virtual void Destroy() = 0;
		virtual void Delete() = 0;
		virtual bool IsAlive() = 0;
		virtual void OnAnimationAttached(engine::hades::Animation* anim) = 0;
		virtual void unk8() = 0;
		virtual void RunScriptAction() = 0; // Not fully reversed yet! Don't use.
		virtual void TeleportTo(D3DXVECTOR2*, bool, bool) = 0;
		virtual void unk9() = 0;
		virtual void unk10() = 0;
		virtual void AttachTo(Thing*, D3DXVECTOR2*) = 0;
		virtual void Detach() = 0;
		virtual void OnPropertiesDirty() = 0;
		virtual char* ToString() = 0;
		virtual void OnAnimationRemoved(engine::hades::Animation*) = 0;
		virtual void HotLoad(const char*) = 0;
		virtual float GetSpeed(float) = 0;
		virtual void Dispose(bool) = 0;
		virtual void unk11() = 0;
		virtual engine::misc::IRectangle* GetDrawBoundsInternal(engine::misc::IRectangle*) = 0;
		virtual void FirstInit() = 0;
		virtual void Defaults() = 0;
		virtual void Obstacle_Init() = 0;
		virtual void OnSortScoreUpdated() = 0;
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
		DWORD64 pText;
		DWORD64 pMetering;
		DWORD64 pSpeech;
		DWORD64 pInteraction;
		AnimationData* pAnim;
		EntityLinkedObjectThing mAttachedTo;
		LuaTable mAttachedLua;
		std::vector<int> mGridSquares;
		std::vector<int> mAttachmentIds;
		std::vector<Animation*> mFrontAnims;
		std::vector<Animation*> mBackAnims;
		std::optional<Polygon> mGeometry;
		DWORD64 pLight;
		D3DXVECTOR2 mAttachOffset;
		std::string mAsString;
		bool mUseScreenLocation;
		bool mFixGeometryWithZ;
		bool padding0[2];
		int mAmbientSpoundId;
		volatile unsigned int mPrepped;
		float mParallax;
		float mOutlineOpacity;
		float mLastImpulsedAnimTime;
		BYTE mLifeStatus[1];
		class MapThing* pMapThing;
		class UnitManager* pManager;
		int* pRef;
		Polygon mMotionTestingPoly;
		std::optional<TransitionHelper> mAdjustZ;
		std::optional<TransitionHelper> mAdjustParallax;
		std::vector<DWORD64> mGroupNames;
		std::vector<class ThingComponent*> mComponents;
		std::unordered_set<Animation*, std::hash<Animation*>, std::equal_to<Animation*>> mAttachedAnims; // eastl::hash_set
		engine::misc::HashGuid mName;

	public:
		// Game Functions

		void MoveInput(D3DXVECTOR2* location, float speed_fraction, bool strafe, float elapsed_seconds)
		{
			return static_cast<void(__fastcall*)(Thing*, D3DXVECTOR2*, float, bool, float)>((PVOID)engine::addresses::unit::functions::move_input)
				(this, location, speed_fraction, strafe, elapsed_seconds);
		}

		float GetParallaxAmount()
		{
			return static_cast<float(__fastcall*)(Thing*)>((PVOID)engine::addresses::thing::functions::get_parallax_amount)(this);
		}

		engine::misc::HashGuid* GetName_Internal(engine::misc::HashGuid* result)
		{
			PVOID* vtbl = *(PVOID**)this;
			return static_cast<engine::misc::HashGuid * (__fastcall*)(Thing*, engine::misc::HashGuid*)>(vtbl[17])(this, result);
		}

		Animation* SetAnimation(engine::misc::HashGuid name, bool suppress_sounds, bool suppress_sounds_if_invisible)
		{
			return static_cast<Animation * (__fastcall*)(Thing*, engine::misc::HashGuid, bool, bool)>
				((PVOID)engine::addresses::thing::functions::set_animation)(this, name, suppress_sounds, suppress_sounds_if_invisible);
		}

		bool IsAlive_Internal()
		{

			return this->mLifeStatus[0] == 0;
		}

		// Must be called in correct function
		void DrawHightlight(engine::misc::Color col)
		{
			return static_cast<void(__fastcall*)(Thing*, engine::misc::Color)>((PVOID)engine::addresses::thing::functions::draw_highlight)(this, col);
		}

		bool IsInvulnerable()
		{
			if (!this->pAnim || !this->pAnim->mDef.mOwnerInvulnerable)
			{
				if (!this->pLife || !this->pLife->mIsInvulnerable)
					return false;
			}
			return true;
		}

		bool IsTargetable()
		{
			return static_cast<bool(__fastcall*)(Thing*)>((PVOID)engine::addresses::thing::functions::is_targetable)(this);
		}

		// Helper functions

		float Distance(Thing* other)
		{
			auto delta = this->location - other->location;
			return sqrt(delta.x * delta.x + delta.y * delta.y);
		}

		float Distance(D3DXVECTOR2 pos)
		{
			auto delta = this->location - pos;
			return sqrt(delta.x * delta.x + delta.y * delta.y);
		}

		float GetAngle(Thing* other)
		{
			float angle = atan2f((float)(other->location.y - this->location.y) * -1.f, other->location.x - this->location.x);
			float result = floorf(angle / (6.2831855f)); // 2PI
			return angle - (float)(result * (6.2831855f));
		}
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