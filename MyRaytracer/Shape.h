#pragma once
#include <vector>
#include <memory>

#include "Ray.h"
#include "Transform.h"
#include "Material.h"

struct inform
{
	bool outface;
	double distance;
	point3 intersection;
	Vector3 normal;
	Material material;
};

class Shape
{
public:

	Shape() = default;
	virtual ~Shape() = default;
	virtual bool intersect(const Ray& ray, inform& _inform) const = 0;

	static void set_normal(const Ray& ray, Vector3& normal, bool& outface)
	{
		outface = ray.direction.Dot(normal) < 0;
		normal = outface ? normal : -normal;
	}
};

class sphere final : public Shape
{
	double radius_{};
	Material material_;
	Transform transform_self_;
public:
	sphere() = default;
	sphere(const Transform& transform, const double& r, const Material& material)
		: radius_(r), material_(material), transform_self_(transform) {}

	bool intersect(const Ray& ray, inform& inform) const override
	{
		const Vector3 oc = transform_self_.get_pos() - ray.origin;
		const double neg_b = oc.Dot(ray.direction);
		const double c = oc.Dot(oc) - radius_ * radius_;
		double det = neg_b * neg_b - c;
		if (det < 0)
			return false;
		else
			det = sqrt(det);

		constexpr double epsilon = 1e-4;
		if (double t = neg_b - det; t > epsilon && t < inform.distance)
		{
			inform.distance = t;
			inform.material = material_;
			inform.intersection = ray.origin + ray.direction * t;
			inform.normal = inform.intersection - transform_self_.get_pos();
			set_normal(ray, inform.normal.Normalize(), inform.outface);
			return true;
		}
		else if (t = neg_b + det; t > epsilon && t < inform.distance)
		{
			inform.distance = t;
			inform.material = material_;
			inform.intersection = ray.origin + ray.direction * t;
			inform.normal = inform.intersection - transform_self_.get_pos();
			set_normal(ray, inform.normal.Normalize(), inform.outface);
			return true;
		}
		return false;
	}
};

class triangle : public Shape
{
	Vector3 p0_, p1_, p2_;
	Material material_;
public:
	triangle() = default;
	triangle(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Material& material)
		: p0_(p1), p1_(p2), p2_(p3), material_(material) {}

	bool intersect(const Ray& ray, inform& inform) const override
	{
		Vector3 E1 = p1_ - p0_;
		Vector3 E2 = p2_ - p0_;
		const Vector3 P = ray.direction.Cross(E2);
		const double det = E1.Dot(P);
		if (fabs(det) < 1e-5) return false;
		const double invS1E1 = 1. / det;
		const Vector3 T = ray.origin - p0_;
		const double u = invS1E1 * T.Dot(P);
		if (u < 0 || u > 1) return false;

		const Vector3 Q = T.Cross(E1);
		const double v = ray.direction.Dot(Q) * invS1E1;
		if (v < 0 || u + v>1) return false;

		const double t = E2.Dot(Q) * invS1E1;

		if (t < 0)
			return false;

		Vector3 v01 = p1_ - p0_;
		Vector3 v02 = p2_ - p0_;

		inform.normal = v01.Cross(v02).Normalize();
		// if (_inform.normal.Dot(ray.direction) > 0)
			// _inform.normal = -_inform.normal;
		// _inform.outface = true;
		set_normal(ray, inform.normal, inform.outface);
		inform.intersection = ray.origin + ray.direction * t;
		inform.distance = t;
		inform.material = material_;
		return true;

	}
};

class quad : public Shape
{

};

class Shapes : public Shape
{
protected:
	std::vector<std::shared_ptr<Shape>> objects_;
public:
	Shapes() = default;
	explicit Shapes(const std::shared_ptr<Shape>& object) { objects_.push_back(object); }
	void add(const std::shared_ptr<Shape>& object)
	{
		objects_.push_back(object);
	}

	bool intersect(const Ray& ray, inform& inform) const override
	{
		constexpr double infinity = 1e20;
		double min_distance = inform.distance = infinity;

		for (const auto& object : objects_)
		{
			if (object->intersect(ray, inform))
			{
				if (inform.distance < min_distance)
					min_distance = inform.distance;
			}
		}

		return min_distance < infinity;
	}
};

class world : public Shapes
{
protected:
	Transform transform_;
public:
	world() = default;
	~world() override = default;
};