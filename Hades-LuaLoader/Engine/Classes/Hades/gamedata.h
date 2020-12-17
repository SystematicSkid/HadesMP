#pragma once

namespace engine::hades
{
	struct GameData
	{
		virtual void unk1() = 0;
		engine::misc::HashGuid name;
		engine::misc::HashGuid organization;
		engine::misc::HashGuid inherit_from;
	};
}