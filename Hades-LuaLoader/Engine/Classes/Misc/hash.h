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
			return (char*)(hashes + id);
		}
	};
}