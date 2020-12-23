#pragma once

namespace engine
{
	class ScriptManager
	{
	public:
		static DWORD64*& lua_state;
	public:
		static bool LoadFile(const char* name)
		{
			return static_cast<BOOLEAN(__fastcall*)(const char*)>((PVOID)engine::addresses::lua::functions::loadfile)(name);
		}
	};

	DWORD64*& engine::ScriptManager::lua_state = *(DWORD64**)(engine::addresses::lua::state);
}