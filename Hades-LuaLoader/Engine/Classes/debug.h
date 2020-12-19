#pragma once

namespace engine
{
	class Debug
	{
	public:
		// Mostly static methods

		// UI Method

		// Doesn't really work T_T
		static void DrawString(std::string text, D3DXVECTOR2* location, int col, float scale)
		{
			return static_cast<void(__fastcall*)(std::string, D3DXVECTOR2*, int, float)>((PVOID)engine::addresses::debug::functions::ui_draw_string)
				(text, location, col, scale);
		}
	};
}