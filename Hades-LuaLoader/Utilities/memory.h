#pragma once
#include <Psapi.h>
#include <algorithm>
#include <windows.h>
#include <string.h>

typedef uintptr_t ptr;
typedef unsigned int uint;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long ulong;

class Memory
{
public:
	static std::string HexToBytes(std::string hex)
	{
		std::string bytes;

		hex.erase(std::remove_if(hex.begin(), hex.end(), isspace), hex.end());

		for (uint i = 0; i < hex.length(); i += 2)
		{
			if ((uchar)hex[i] == '?')
			{
				bytes += '?';
				i -= 1;

				continue;
			}

			uchar byte = (uchar)std::strtol(hex.substr(i, 2).c_str(), nullptr, 16);
			bytes += byte;
		}

		return bytes;
	}
	static ptr SigScan(const char* pattern, const char* secondary_pattern = nullptr)
	{

		HMODULE mod = GetModuleHandleA("EngineWin64s.dll");
		MODULEINFO info;
		GetModuleInformation(GetCurrentProcess(), mod, &info, sizeof(info));

		uchar* base = (uchar*)mod;

		std::string signature = HexToBytes(pattern);

		uchar first = (uchar)signature.at(0);
		uchar* end = (base + info.SizeOfImage) - signature.length();

		for (; base < end; ++base)
		{
			if (*base != first)
				continue;

			uchar* bytes = base;
			uchar* sig = (uchar*)signature.c_str();

			for (; *sig; ++sig, ++bytes)
			{
				if (*sig == '?')
					continue;

				if (*bytes != *sig)
					goto end;
			}

			if (!base)
			{
				return SigScan(secondary_pattern);

			}

			return (ptr)base;

		end:;
		}

		return NULL;
	}

	static ptr GetCallAddress(const char* pattern, const char* secondary_pattern = nullptr)
	{
		auto address = Memory::SigScan(pattern);
		if (!address)
		{
			address = Memory::SigScan(secondary_pattern);
		}
		uintptr_t call_addr = (address + *(signed long*)(address + 1) + 5);
		return call_addr;
	}

	static ptr GetInstanceAddress(const char* pattern)
	{
		auto address = Memory::SigScan(pattern);
		return (ptr)(address + *(signed long*)(address + 3) + 7);
	}
};