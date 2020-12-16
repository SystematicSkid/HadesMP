#pragma once
#include <functional>
#include <d3d11.h>

namespace core::hooks::directx
{
	/* Our Callback */
	std::function<VOID(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)> OnPresent = nullptr;

	/* Address of original functions */
	PVOID original_present = NULL;

	/* Raw callback of `DirectX->EndScene` */
	HRESULT __stdcall hook_present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		//auto protection_object = present_hook->GetProtectionObject();
		// Call event `Hooks::DirectX->OnEndScene`
		if (hooks::directx::OnPresent)
		{
			hooks::directx::OnPresent(pSwapChain, SyncInterval, Flags);
		}

		// Return original function
		return static_cast<HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT)>((PVOID)original_present)(pSwapChain, SyncInterval, Flags);
	}

	void initialize()
	{
		auto wnd = (HWND)FindWindowA("The Forge", NULL);
		auto pSwapChain = engine::Renderer::Instance->context;
		DWORD64 myFunc = (DWORD64)hook_present;
		DWORD64* engineInterface = (DWORD64*)*(DWORD64*)pSwapChain;

		if (engineInterface[8] != myFunc)
		{
			//Allocate some memory for our copy of the interface (vtable)
			static DWORD64 myEngineInterface[208 + 150];

			DWORD64* engineInterface = (DWORD64*)*(DWORD64*)pSwapChain;

			//Copy from the original to ours...
			memcpy(&myEngineInterface, engineInterface, sizeof(myEngineInterface));
			*(DWORD64*)pSwapChain = (DWORD64)&myEngineInterface;
			//Now apply the hook to our interface.
			original_present = (PVOID)myEngineInterface[8];
			myEngineInterface[8] = (DWORD64)myFunc;
		}
	}
}