#pragma once

namespace engine::hades
{

	struct MapThingGroupId
	{
		engine::misc::HashGuid mName;

		// Constructor
		MapThingGroupId(const char* name)
		{
			this->mName = engine::misc::HashGuid(name);
		}
	};

	struct MapThingGroup
	{
		MapThingGroupId mId;
		bool mVisible;
		int mBlendMode;
		MapThingGroupId mParentGroup;
		std::vector<MapThingGroupId> mChildGroups;
		//eastl::hash_map<int, MapThing*, eastl::hash<int>, eastl::equal_to<int>> mThings;
		char pad[0x30];
		int mNumThings;
		int mNumThingsSelectable;
		int mNumThingsVisible;
		int mNumThingsFrozen;
		int mNumSelectedAsLogic;
		int mNumSelectedAsDraw;
	};

	class GroupManager
	{
	public:
		//eastl::hash_map < engine::hades::MapThingGroupId, int, eastl::hash<engine::hades::MapThingGroupId>, eastl::equal_to<>> mLookup;
		char pad[0x30];
		std::vector<engine::hades::MapThingGroup> mWorking;
		std::vector<engine::hades::MapThingGroup> mGroups;

	public:
		// Methods

		MapThingGroup* Get(const char* name)
		{
			// Our own wrapper for the engine function
			MapThingGroupId id(name);
			return static_cast<MapThingGroup * (__fastcall*)(GroupManager*, MapThingGroupId*)>((PVOID)engine::addresses::groupmanager::functions::get_groupid)
				(this, &id);
		}
	};
}