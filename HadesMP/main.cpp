#include <string>
#include <fstream>
#include "./Core/core.h"

HMODULE dll;
Proxy proxy;
HWND g_hwnd;

core::network::Client core::network::client;
core::network::Server core::network::server;

void init()
{
	FILE* pNewStdout = NULL;
	FILE* pNewStderr = NULL;
	FILE* pNewStdin = NULL;

	/* Initialize our output console */
	/* TODO: Move this into a debug macro */
	AllocConsole();
	SetConsoleTitleA("HadesMP");
	/* Assign `stdin`, `stout`, `stderr`  */
	::freopen_s(&pNewStdin, "CONIN$", "r", stdin);
	::freopen_s(&pNewStdout, "CONOUT$", "w", stdout);
	::freopen_s(&pNewStderr, "CONOUT$", "w", stderr);

	/* Global window hwnd */
	g_hwnd = (HWND)FindWindowA("The Forge", NULL);

	if (!core::initialize())
	{
		MessageBoxA(NULL, "Error: Could not initialize core!", NULL, NULL);
		return; // Maybe free and exit?
	}

	//FreeLibraryAndExitThread(dll, 0x1); // We're done, unload
}

bool WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
	if (_Reason != DLL_PROCESS_ATTACH)
		return FALSE;
	dll = (HMODULE)_DllHandle;
	//DisableThreadLibraryCalls((HMODULE)_DllHandle);
	init();
	//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)init, NULL, NULL, NULL);
	return TRUE;
}