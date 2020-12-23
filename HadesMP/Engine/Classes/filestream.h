#pragma once

namespace engine
{
	class FileStream
	{
	public:
		/* Virtual method table */
		struct  IFileSystem
		{
			virtual void Open() = 0;
			virtual void Close() = 0; // done
			virtual DWORD64 Read(char* buffer, size_t length) = 0; // done
			virtual void Write() = 0;
			virtual void Seek() = 0;
			virtual void GetSeekPosition() = 0;
			virtual size_t GetFileSize(FileStream*) = 0; // done
			virtual void Flush() = 0;
			virtual void IsAtEnd() = 0;
			virtual void GetResourceMount() = 0;
			virtual void pUser() = 0;
		}*pIO;
	public:
		union /* anonymous union for the stream */
		{
			// TODO: Add pFile, memory, and user types
			char pad[0x18];
		};
		DWORD64 size;
		DWORD mode;
		char pad_28[0x4];
	public:
		size_t GetSize()
		{
			PVOID* vtable = (PVOID*)this;
			return static_cast<size_t(__fastcall*)(FileStream*)>(vtable[6])(this);
		}

		DWORD64 read(char* buffer, size_t len)
		{
			PVOID* vtable = (PVOID*)this;
			return static_cast<DWORD64(__fastcall*)(FileStream*, char*, size_t)>(vtable[2])(this, buffer, len);
		}

	};
} // Size: 0x30