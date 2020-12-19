#pragma once

namespace engine
{
	struct __declspec(align(8)) LuaTable
	{
		struct lua_State* state;
		int ref;
	};
}