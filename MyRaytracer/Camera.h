#pragma once
#include "Ray.h"
#include "Transform.h"

class Camera
{
	Vector2i resolution_;
	Vector3 direction_;
	Vector3 left_;
	Vector3 up_;
	double inv_width_;
	double inv_height_;
	double fov_;
	Transform transform_;
public:
	Camera() = default;
	Camera(const Transform& transform, const Vector3& direction, double fov, Vector2i resolution)
		: resolution_(resolution), direction_(direction), fov_(fov), transform_(transform)
	{
		inv_width_ = 1. / resolution.x;
		inv_height_ = 1. / resolution.y;
	}

	Ray GenerateRay(int x, int y, double dx, double dy);
		
	Vector2i get_res() const;
	
};

inline Ray Camera::GenerateRay(int x, int y, double dx, double dy)
{
	static double aspect_ratio = inv_height_ / inv_width_;
	static double tan_fov = tan(PI * 0.5 * fov_ / 180);
	left_ = tan_fov * aspect_ratio;
	up_ = left_.Cross(direction_).Normalize() * tan_fov;
	Vector3 dir = left_ * (((x + 0.5 + dx) * inv_width_) * 2 - 1) + up_ * (1 - ((y + 0.5 + dy) * inv_height_) * 2) + direction_;
	return Ray(transform_.get_pos(), dir.Normalize());
}

inline Vector2i Camera::get_res() const
{
	return resolution_;
}
