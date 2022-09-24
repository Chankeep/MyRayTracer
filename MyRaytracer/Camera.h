#pragma once
#include "Ray.h"

class Camera
{
public:
	Camera(const Vector3& position, const Vector3& direction, double fov, int Width, int Height)
		: position(position), direction(direction), fov(fov)
	{
		invWidth = 1. / Width;
		invHeight = 1. / Height;
	}

	Ray GenerateRay(int x, int y, double dx, double dy)
	{
		static double aspect_ratio = invHeight / invWidth;
		static double tan_fov = tan(PI * 0.5 * fov / 180);
		left = tan_fov * aspect_ratio;
		up = left.Cross(direction).Normalize() * tan_fov;
		Vector3 dir = left * (((x + 0.5 + dx) * invWidth) * 2 - 1) + up * (1 - ((y + 0.5 + dy) * invHeight) * 2) + direction;
		return Ray(position, dir.Normalize());
	}
	Vector3 position;
	Vector3 direction;
	Vector3 left;
	Vector3 up;
	double invWidth;
	double invHeight;
	double fov;
};