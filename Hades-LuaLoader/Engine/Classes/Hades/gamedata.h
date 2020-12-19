#pragma once

namespace engine::hades
{
	struct GameData
	{
		struct  
		{
			void(__fastcall* unk1)(GameData*);
		}*vtbl;
		engine::misc::HashGuid name;
		engine::misc::HashGuid organization;
		engine::misc::HashGuid inherit_from;
	};
}