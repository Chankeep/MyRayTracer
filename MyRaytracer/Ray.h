#pragma once
#include "Vector.h"

class Ray
{
public:
	Ray(const Vector3& ori, const Vector3& dir) : origin(ori), direction(dir) {}

	Vector3 origin;
	Vector3 direction;
};