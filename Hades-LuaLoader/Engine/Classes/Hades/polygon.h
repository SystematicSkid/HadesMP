#pragma once

namespace engine::hades
{
	struct Polygon
	{
		float mHighestSideYValue;
		float mRadius;
		D3DXVECTOR2 mCenter;
		unsigned __int64 mMinXSideIndex;
		std::vector<D3DXVECTOR2> mPoints;
	};
}