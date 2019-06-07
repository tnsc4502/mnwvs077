#pragma once
#include <cmath>

struct FieldPoint
{
	int x = 0, y = 0;
	FieldPoint() {} 
	FieldPoint(int x_, int y_) { x = x_; y = y_; }

	double Range(FieldPoint& pt)
	{
		return std::sqrt(std::pow((double)(pt.x - x), 2) + std::pow((double)(pt.y - y), 2));
	}
};