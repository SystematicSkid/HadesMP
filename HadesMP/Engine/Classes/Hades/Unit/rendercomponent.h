#pragma once

namespace engine::hades
{
	class RenderComponent : public engine::Rtti
	{
	private:
		// Pure calls
		virtual void unk1() = 0;
		virtual void unk2() = 0;
		virtual void unk3() = 0;
		virtual void unk4() = 0;
		virtual engine::misc::Bounds* GetBounds(engine::misc::Bounds* result) = 0;
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