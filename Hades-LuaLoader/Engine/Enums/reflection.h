#pragma once

namespace engine
{
	enum Reflection_ValueChangeType
	{
		Absolute = 0x0,
		Multiply = 0x1,
		MultiplyBase = 0x2,
		Add = 0x3,
		ReplaceIfNull = 0x4
	};
}