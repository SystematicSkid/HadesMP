#pragma once
/* STL */
#include <cstdint>
#include <string.h>
#include <vector>
#include <optional>
#include <unordered_map>

/* Windows */
#include <Windows.h>
#include <winternl.h>
#include <Xinput.h>
#include <comdef.h>
#include <minwindef.h>
#include <winbase.h>
#include <intrin.h>
#include <cor.h>
#include <cordebug.h>
#include <metahost.h>
#include <psapi.h>

/* DirectX */
#include <d3d11.h>
#include <d3d9.h>
#include <d3dx9math.h>

/* Utilities */
#include "../Utilities/memory.h"

/* Addresses */
#include "./addresses.h"

/* Enums */
#include "./Enums/filesystem.h"
#include "./Enums/rtti.h"
#include "./Enums/map.h"

/* Utility Classes */
#include "./Classes/Misc/rectangle.h"
#include "./Classes/Misc/hash.h"
#include "./Classes/Misc/soundcue.h"
#include "./Classes/Misc/color.h"
#include "./Classes/Misc/bounds.h"

/* Classes */
#include "./Classes/rtti.h"
#include "./Classes/renderer.h"
#include "./Classes/scriptmanager.h"
#include "./Classes/filestream.h"
#include "./Classes/platformfile.h"
#include "./Classes/app.h"
#include "./Classes/debug.h"

#include "./Classes/Hades/gamedata.h"

/* Game Classes */
#include "./Classes/Hades/Unit/rendercomponent.h"
#include "./Classes/Hades/Unit/animation.h"
#include "./Classes/Hades/Unit/components.h"
#include "./Classes/Hades/Unit/thing.h"
#include "./Classes/Hades/Unit/weapon.h"
#include "./Classes/Hades/Unit/pathfinding.h"
#include "./Classes/Hades/Unit/unitdata.h"
#include "./Classes/Hades/Unit/unit.h"
#include "./Classes/Hades/Unit/playerunit.h"

/* World Classes */
#include "./Classes/Hades/World/map.h"
#include "./Classes/Hades/World/world.h"

#include "./Classes/Hades/camera.h"
#include "./Classes/Hades/player.h"
#include "./Classes/Hades/playermanager.h"
#include "./Classes/Hades/unitmanager.h"

namespace global
{
	engine::hades::UnitData* new_unit_data = nullptr;
	engine::hades::MapThing* new_map_thing = nullptr;
	D3DXVECTOR2 spawn_location;

	engine::hades::Thing* replicated_unit = nullptr;
	engine::hades::Thing* original_unit = nullptr;


}