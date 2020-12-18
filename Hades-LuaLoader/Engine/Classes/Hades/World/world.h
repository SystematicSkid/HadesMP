#pragma once

namespace engine::hades
{
	class __declspec(align(16)) World
	{
	public:
		static World*& Instance;
	public:
		bool mRunning;
		bool mPaused;
		bool mResetAppOnBegin;
		bool mResetWorldOnBegin;
		bool mResetBinksOnBegin;
		bool mResetWeaponBinksOnBegin;
		bool mUsePreDrawThread;
		bool mIsInited;
		bool mIsPausedAtLuaBreakPoint;
		bool mResetting;
		bool mRequestResume;
		bool mStepFrame;
		int mFrame;
		int mNextObstacleId;
		int mNextUnitId;
		BYTE mBeginRequest[1];
		float mElapsedTime;
		float mSpeedModifier;
		float mSpeedModifierTarget;
		float mSpeedModifierChangeSpeed;
		float mSpeedModifierDuration;
		float SessionWorldTime;
		engine::misc::Color mLoadBackgroundColor;

		// TODO: Finish adding fields

		// Methods
		void LoadNextMap(std::string* map_name, int begin_request, engine::misc::Color load_color)
		{
			return static_cast<void(__fastcall*)(World*, std::string*, int, engine::misc::Color)>
				((PVOID)engine::addresses::world::functions::load_next_map)(this, map_name, begin_request, load_color);
		}
	};

	engine::hades::World*& engine::hades::World::Instance = *(engine::hades::World**)(engine::addresses::world::instance);
}