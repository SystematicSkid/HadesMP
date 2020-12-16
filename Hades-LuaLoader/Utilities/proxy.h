#pragma once
#include <windows.h>
#include <Xinput.h>

class Proxy
{
private:

	HMODULE module; //xinput1_3.dll
private:
	template<class T>
	T set_call(T& ptr, const char* name)
	{
		if (!ptr)
			ptr = (T)GetProcAddress(module, name);

		return ptr;
	}

	template<class T>
	T set_call(T& ptr, int ord)
	{
		if (!ptr)
			ptr = (T)GetProcAddress(module, (const char*)ord);

		return ptr;
	}

#pragma warning( disable: 4995 )
	void WINAPI XInputEnable(BOOL state)
	{
		static void (WINAPI * orig)(BOOL state);
		return set_call(orig, "XInputEnable")(state);
	}

	DWORD WINAPI XInputGetBatteryInformation(DWORD dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation)
	{
		static DWORD(WINAPI * orig)(DWORD dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION * pBatteryInformation);
		return set_call(orig, "XInputGetBatteryInformation")(dwUserIndex, devType, pBatteryInformation);
	}
	DWORD WINAPI XInputGetAudioDeviceIds(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid)
	{
		static DWORD(WINAPI * orig)(DWORD dwUserIndex, GUID * pDSoundRenderGuid, GUID * pDSoundCaptureGuid);
		return set_call(orig, "XInputGetAudioDeviceIds")(dwUserIndex, pDSoundRenderGuid, pDSoundCaptureGuid);
	}
	DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities)
	{
		static DWORD(WINAPI * orig)(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES * pCapabilities);
		return set_call(orig, "XInputGetCapabilities")(dwUserIndex, dwFlags, pCapabilities);
	}

	DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, PXINPUT_KEYSTROKE pKeystroke)
	{
		static DWORD(WINAPI * orig)(DWORD dwUserIndex, DWORD dwReserved, PXINPUT_KEYSTROKE pKeystroke);
		return set_call(orig, "XInputGetKeystroke")(dwUserIndex, dwReserved, pKeystroke);
	}

	DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
	{
		static DWORD(WINAPI * orig)(DWORD dwUserIndex, XINPUT_STATE * pState);
		return set_call(orig, "XInputGetState")(dwUserIndex, pState);
	}

	DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
	{
		static DWORD(WINAPI * orig)(DWORD dwUserIndex, XINPUT_VIBRATION * pVibration);
		return set_call(orig, "XInputSetState")(dwUserIndex, pVibration);
	}

	DWORD WINAPI XInputOrd100(DWORD dwUserIndex, XINPUT_STATE* pState)
	{
		static DWORD(WINAPI * orig)(DWORD dwUserIndex, XINPUT_STATE * pState);
		return set_call(orig, 100)(dwUserIndex, pState);
	}

	DWORD WINAPI XInputOrd101(DWORD dwUserIndex, DWORD unk, void* ptr)
	{
		static DWORD(WINAPI * orig)(DWORD dwUserIndex, DWORD unk, void* ptr);
		return set_call(orig, 101)(dwUserIndex, unk, ptr);
	}

	DWORD WINAPI XInputOrd102(DWORD dwUserIndex)
	{
		static DWORD(WINAPI * orig)(DWORD dwUserIndex);
		return set_call(orig, 102)(dwUserIndex);
	}

	DWORD WINAPI XInputOrd103(DWORD dwUserIndex)
	{
		static DWORD(WINAPI * orig)(DWORD dwUserIndex);
		return set_call(orig, 103)(dwUserIndex);
	}
	DWORD WINAPI XInputOrd104(DWORD dwUserIndex)
	{
		static DWORD(WINAPI * orig)(DWORD dwUserIndex);
		return set_call(orig, 104)(dwUserIndex);
	}
	DWORD WINAPI XInputOrd108(DWORD dwUserIndex)
	{
		static DWORD(WINAPI * orig)(DWORD dwUserIndex);
		return set_call(orig, 108)(dwUserIndex);
	}
public:
	bool Init()
	{
		char path[MAX_PATH];;
		GetSystemDirectoryA(path, MAX_PATH);
		strcat_s(path, "\\XInput1_4.dll");
		module = LoadLibraryA(path);
		return !!module;
	}
};

extern Proxy proxy;