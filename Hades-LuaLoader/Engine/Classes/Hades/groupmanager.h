#pragma once
#include <unordered_map>

namespace engine::hades
{

	struct MapThingGroupId
	{
		engine::misc::HashGuid mName;
	};

	class GroupManager
	{
	public:
		std::unordered_map < engine::hades::MapThingGroupId, int, std::hash<engine::hades::MapThingGroupId>, std::equal_to<>> mLookup;
		std::vector<engine::hades::MapThingGroupId> mWorking;
		std::vector<engine::hades::MapThingGroupId> mGroups;
	};
}