#pragma once

namespace engine
{
	class PlatformFile
	{
	public:
		static bool CreateStream(engine::ResourceDirectory directory, const char* filename, engine::FileMode mode, engine::FileStream* stream_out)
		{
			// TODO: Reverse the filestream class
			return static_cast<BOOLEAN(__fastcall*)(engine::ResourceDirectory, const char*, engine::FileMode, engine::FileStream*)>
				((PVOID)engine::addresses::platformfile::functions::createstream)(directory, filename, mode, stream_out);
		}

		static char* GetResourceDirectory(engine::ResourceDirectory directory)
		{
			return (char*)((((uintptr_t)engine::module_engine + 0x1CCF3D0) + (int32_t)directory * 0x210) - 0x8);
		}
	};
}