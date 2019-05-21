#pragma once
#include "FieldPoint.h"

struct FieldRect
{
	int left = 0, top = 0, right = 0, bottom = 0;

	bool PtInRect(const FieldPoint& pt)
	{
		return (pt.x >= left && pt.x <= right) && (pt.y <= bottom && pt.y >= top);
	}
};