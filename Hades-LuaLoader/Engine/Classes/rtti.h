#pragma once

namespace engine
{
	class __declspec(align(8)) Rtti
	{
		virtual void* __vecDelDtor(unsigned int) = 0;
		engine::RttiType mType;
	};
}