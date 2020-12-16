#pragma once
/* STL */
#include <cstdint>
#include <string.h>
#include <vector>

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

/* Utilities */
#include "../Utilities/memory.h"

/* Addresses */
#include "./addresses.h"

/* Enums */
#include "./Enums/filesystem.h"

/* Classes */
#include "./Classes/scriptmanager.h"
#include "./Classes/filestream.h"
#include "./Classes/platformfile.h"