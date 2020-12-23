#pragma once

namespace engine::hades
{
	struct MapData
	{
		bool mCreated;
		engine::hades::MapSaveStatus SaveStatus;
		std::string mName;
		engine::misc::IRectangle mArea;
		D3DXVECTOR2 mTimeLapseCameraLocation;
		float mTimeLapseCameraZoom;
		float mBrightness;
		std::vector<DWORD64> mThingGroups;
		engine::misc::Color mBackdropColor;
		D3DXVECTOR3 mAmbientLightColor;
	};

	class Map
	{
	public:
		D3DXVECTOR2 mAverageThingLocation;
		D3DXVECTOR3 mAmbientLightColor;
		MapData* mData;
		std::string mPreviousMapName;

	public:
		// Methods
		
		// Will not work if not used properly!
		static void LoadMap(std::string name)
		{
			return static_cast<void(__fastcall*)(std::string)>((PVOID)engine::addresses::map::functions::load_map)(name);
		}
	};
}