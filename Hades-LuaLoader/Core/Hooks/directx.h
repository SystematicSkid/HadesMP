#pragma once
#include <functional>
#include <d3d11.h>

namespace core::hooks::directx
{
	/* Our Callback */
	std::function<VOID(IDXGISwapChain* pSwapChain)> on_present = nullptr;

	/* Address of original functions */
	PVOID original_present = NULL;

	/* Raw callback of `DirectX->OnPresent` */
	DWORD64 __fastcall hook_present(IDXGISwapChain* pSwapChain, DWORD64 a2, DWORD64 a3, DWORD64* a4)
	{
		// Call event `Hooks::DirectX->OnPresent`
		if (hooks::directx::on_present)
			hooks::directx::on_present(pSwapChain);
		// Return original function
		return static_cast<DWORD64(__fastcall*)(IDXGISwapChain*, DWORD64,DWORD64, DWORD64*)>((PVOID)original_present)(pSwapChain, a2,a3,a4);
	}

	void initialize()
	{
		DWORD64* pSwapChain = engine::SwapChain::Instance->pDxSwapChain1;
		DWORD64 myFunc = (DWORD64)hook_present;
		DWORD64* engineInterface = (DWORD64*)*(DWORD64*)pSwapChain;

		if (engineInterface[0x16] != myFunc)
		{
			//Allocate some memory for our copy of the interface (vtable)
			static DWORD64 myEngineInterface[208 + 150];

			DWORD64* engineInterface = (DWORD64*)*(DWORD64*)pSwapChain;

			//Copy from the original to ours...
			memcpy(&myEngineInterface, engineInterface, sizeof(myEngineInterface));
			*(DWORD64*)pSwapChain = (DWORD64)&myEngineInterface;
			//Now apply the hook to our interface.
			original_present = (PVOID)myEngineInterface[0x16];
			myEngineInterface[0x16] = (DWORD64)myFunc;
		}
	}
}