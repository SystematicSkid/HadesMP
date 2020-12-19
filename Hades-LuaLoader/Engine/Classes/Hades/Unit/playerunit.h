#pragma once

namespace engine::hades
{

	class PlayerUnitInput
	{
	public:
		class PlayerUnit* pOwner;
		bool mCanMove;
		bool mStopAtPathEnd;
		bool mUsingMouse;
		bool mUsingDirectControl;
		float mManualTargetingDelay;
		float mManualTargetingAcceleration;
		float mManualTargetingTime;
		float mReMagnetizeTimeOut;
		float mLockMovementAngle;
		D3DXVECTOR2 mMoveTarget;
		PlayerPathFinding* pPlayerPathfinding;
		EntityLinkedObjectThing mLastMouseoverTarget;
		EntityLinkedObjectThing mLockTarget;
		EntityLinkedObjectThing mLastMagnetizedTarget;
		std::vector<Weapon*> mLostControlWeapons;
	};

	class PlayerUnit : public Unit
	{
	public:
		std::unique_ptr<PlayerUnitInput> pInput;

	public:
		// Methods
		//void MoveTo(D3DXVECTOR2* destination, )
	};
}

