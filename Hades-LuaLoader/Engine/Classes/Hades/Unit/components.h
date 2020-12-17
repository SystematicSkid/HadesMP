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
}