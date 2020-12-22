#pragma once

namespace engine::hades
{
	enum class MapSaveStatus
	{
	  Unsaved = 0x0,
	  Map = 0x2,
	  Scripts = 0x4,
	  Saved = 0x6,
	};
}