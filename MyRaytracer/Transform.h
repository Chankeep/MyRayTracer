#pragma once
#include "Vector.h"

class Transform
{
	point3 position;
	Vector3 rotation;
	Vector3 scale;
public:
	Transform() = default;
	Transform(const Vector3& pos) : position(pos) {  }

	point3 get_pos() const;
	void set_pos(const Vector3& pos);
};

inline point3 Transform::get_pos() const
{
	return position;
}

inline void Transform::set_pos(const Vector3& pos)
{
	position = pos;
}
