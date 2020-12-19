#pragma once

namespace engine::hades
{
	// Misc.
	struct __declspec(align(4)) Target
	{
		D3DXVECTOR2 mTargetLocation;
		int mTargetThingId;
		BYTE mMode[1];
	};

	struct __declspec(align(4)) MoveAIData : public GameData
	{
		bool mStopToRotate;
		bool mSlowToRotate;
		bool mMoveWithoutPath;
		bool mStayOnCamera;
		bool mFinishAtGoal;
		bool mRequiresLoS;
		bool mIgnoreTargetForLoS;
		BYTE mAlgorithm[1];
		float mNodeDistance;
		float mAlternateNodeDistance;
		float mNodeSuccessDistance;
		float mNodeSuccessDistanceRequiresLoS;
		float mLookAheadDistance;
		float mLookBehindDistance;
		float mFailedPathfindTimeout;
		float mRepathDelayMin;
		float mRepathDelayMax;
		float mRepathTargetMoveDistance;
		float mRepathTimeout;
		float mAvoidLocationDistance;
		float mSuccessDistance;
		float mLookAheadMultiplier;
		float mTargetHeadingShift;
		int mNodeCountLimit;
		int mSkippedNodeLimit;
		BYTE mOnFailBehavior[1];
	};

	struct __declspec(align(4)) PathfinderNode
	{
		D3DXVECTOR2 mPosition;
		int mDistSoFar;
		int mDistToGoal;
		bool mAlternateDistance;
	};

	struct PlayerPathFinding
	{
		bool mLastMoveGotPath;
		BYTE mLastPathResult[1];
		float mPathAge;
		int mCurrentPathNode;
		D3DXVECTOR2 mLastMoveTarget;
		class PlayerUnit* mOwner;
		PathfinderNode mLastMoveTargetResult;
		std::vector<PathfinderNode> mPathNodes;
		std::vector<PathfinderNode> mPath;
		MoveAIData mMoveData;
	};
}