#pragma once

namespace engine::misc
{
	struct HashGuid
	{
		unsigned int id;

		// Methods
		char* ToString()
		{
			DWORD64 hashes = *(DWORD64*)(engine::addresses::hash::instance);
			return (char*)((uintptr_t)hashes + id);
		}

		static unsigned int StringIntern(const char* s)
		{
			return static_cast<unsigned int(__fastcall*)(const char*)>((PVOID)engine::addresses::hash::functions::string_intern)(s);
		}
	};
}