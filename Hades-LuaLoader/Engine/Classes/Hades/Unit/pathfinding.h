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

	class PlayerPathFinding
	{
	public:
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

	public:
		// Methods
		D3DXVECTOR2* GetNextPathStep(D3DXVECTOR2* result, D3DXVECTOR2 target)
		{
			return static_cast<D3DXVECTOR2 * (__fastcall*)(PlayerPathFinding*, D3DXVECTOR2*, D3DXVECTOR2)>((PVOID)engine::addresses::playerpathfinding::functions::get_next_path_step)
				(this, result, target);
		}
	};


	class PathFinder
	{
	public:
		// Methods

		// The first arg should be Unit!
		static bool CalcPath(Thing* unit, D3DXVECTOR2 goal, Thing* goal_thing, float success_distance)
		{
			return static_cast<bool(__fastcall*)(Thing*, D3DXVECTOR2, Thing*, float)>
				((PVOID)engine::addresses::pathfinder::functions::calc_path)(unit, goal, goal_thing, success_distance);
		}
	};
}