#include <windows.h>
#include <string>
#include <fstream>
#include "./Engine/engine.h"
#include "./Core/Hooks/hooks.h"
#include "./Utilities/proxy.h"

HMODULE dll;
Proxy proxy;
bool init_lua = true;

void init()
{
	/*
	DWORD64 stream_ptr;
	if (!engine::PlatformFile::CreateStream(engine::ResourceDirectory::RD_MIDDLEWARE_2, "mod_order.txt", engine::FileMode::FM_READ_BINARY, (engine::FileStream*)&stream_ptr))
		MessageBoxA(NULL, "Error", "Failed to create filestream!", NULL);
	engine::FileStream* stream = (engine::FileStream*)stream_ptr; // TODO: Find a better way to do this T_T
	*/

	FILE* pNewStdout = NULL;
	FILE* pNewStderr = NULL;
	FILE* pNewStdin = NULL;

	/* Initialize our output console */
	// Allocate console
	AllocConsole();
	SetConsoleTitleA("Hades");
	// Assign `stdin`, `stout`, `stderr`
	::freopen_s(&pNewStdin, "CONIN$", "r", stdin);
	::freopen_s(&pNewStdout, "CONOUT$", "w", stdout);
	::freopen_s(&pNewStderr, "CONOUT$", "w", stderr);

	if (!proxy.Init())
		MessageBoxA(NULL, "An error occurred during initialization!", "Fatal Error", MB_ICONASTERISK);

	core::hooks::on_update = [=]()
	{
		if(init_lua)
		{
			char* dir = engine::PlatformFile::GetResourceDirectory(engine::ResourceDirectory::RD_MIDDLEWARE_2);
			std::string script_dir(dir);
			std::string order_file = script_dir + "\\mod_order.txt";


			std::streampos fsize = 0;
			std::ifstream file(order_file);

			std::string file_name;
			bool result = true;
			while (std::getline(file, file_name))
			{
				bool result = engine::ScriptManager::LoadFile(file_name.c_str());
				if (!result)
					MessageBoxA(NULL, "Failed to load lua file", "Error", NULL);
			}
			init_lua = false;
		}
	};

	core::hooks::initialize();

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