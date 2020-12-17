#pragma once

namespace engine
{
	class __declspec(align(4)) App
	{
	public:
		std::string mLastScreenshotPath;
		bool mIsActive;
		bool mIsFullScreen;
		bool mIsFixedTimeStep;
		bool mShouldCaptureNextFrame;
		_RTL_CRITICAL_SECTION mShouldCaptureNextFrameMutex;
		bool mWasLastFrameCaptured;
		bool mInitialLoadComplete;
		bool mCommandLineReset;
		bool mInUpdate;
		bool mInDraw;
		bool mIsReady;
		bool mBoostCpu;
		bool mIsRunning;
		bool mIsDisposed;
		bool mWasActive;
		bool mIsLastUpdate;
		bool mRequestResize;
		bool mCanUpdateAndDraw;
		int mSwapInterval;
		int mInternalLoadingSkipRenderFrames;
		int mSkippedFrames;
		int mRequestScreenWidth;
		int mRequestScreenHeight;
		int mLoadingSkipRenderFrames;
		float mAccum;
		float mTimeScale;
		float mInternalTargetElapsedTime;
		float mTargetElapsedTime;
	};
}