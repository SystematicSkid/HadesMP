#pragma once

namespace engine::hades
{
	class RenderComponent : public engine::Rtti
	{
	public:
		uint8_t draw_group;
		DWORD64 sort_score;
		int sort_mode;
		unsigned int mSecondarySortKey;
		bool mHasOutline;
		DWORD entity;
		engine::misc::Bounds bounds;
	}; // size: 0x48
}