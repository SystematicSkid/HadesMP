#pragma once

namespace engine
{
	class __declspec(align(8)) Rtti
	{
	private:
		virtual void* __vecDelDtor(unsigned int) = 0;
	public:
		engine::RttiType mType;
	};
}