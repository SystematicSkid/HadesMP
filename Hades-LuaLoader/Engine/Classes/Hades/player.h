#pragma once

namespace engine::hades
{
	class Player
	{
	public:
		bool was_any_input_given;
		bool allow_input;
		bool is_connected;
		bool pause_requested;
		int controller_index;
		int index;
		D3DXVECTOR2 cursor_location;
		DWORD64 active_unit;
		DWORD64 ai;
		// TODO: Maybe finish this?
	};
}