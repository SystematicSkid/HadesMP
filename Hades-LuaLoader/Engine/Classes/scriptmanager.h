#pragma once

namespace engine
{
	class ScriptManager
	{
	public:
		static bool LoadFile(const char* name)
		{
			return static_cast<BOOLEAN(__fastcall*)(const char*)>((PVOID)engine::addresses::lua::functions::loadfile)(name);
		}
	};
}