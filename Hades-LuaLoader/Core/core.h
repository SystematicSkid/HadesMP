#define WIN32_LEAN_AND_MEAN
// win
#include <Windows.h>
#include <winternl.h>
#include <Xinput.h>
#include <d3d11.h>
#include <d3d9.h>
#include <D3DX10math.h>
#include <minwindef.h>
#include <winbase.h>
#include <intrin.h>
#include <psapi.h>

#pragma comment(lib, "d3d11.lib")

// stl
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
#include <clocale>
#include <algorithm>
#include <sstream>
#include <functional>
#include <cassert>

/* Engine Access */
#include "../Engine/engine.h"

/* Utilities */
#include "../Utilities/memory.h"
#include "../Utilities/proxy.h"


/* Hooks */
#include "./Hooks/hooks.h"

/* Renderer */
#include "./Renderer/renderer.h"

namespace core
{
	/* Possibly add core initialization here */
}