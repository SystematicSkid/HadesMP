#pragma once

namespace engine::misc
{
	class any
	{
	private:
		char pad[0x20];
		PVOID handler;
	private:
		const type_info& GetType()
		{
			return reinterpret_cast<const type_info&(__fastcall*)(int, any*)>(handler)(4, this);
		}
	public:
		bool is_bool()
		{
			return this->GetType() == typeid(bool);
		}

		bool is_float()
		{
			return this->GetType() == typeid(float);
		}

		bool is_int()
		{
			return this->GetType() == typeid(int);
		}

		float* get_float()
		{
			return reinterpret_cast<float* (__fastcall*)(int, any*, int)>(this->handler)(0, this, 0);
		}
	};
}