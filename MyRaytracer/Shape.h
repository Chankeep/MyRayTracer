#pragma once
#include <vector>
#include <memory>
#include "Ray.h"

enum class MaterialType { diffuse, specular, Refract };

struct inform
{
	bool outface;
	double distance;
	point3 position;
	Vector3 normal;
	color albedo;
	color emission;
	MaterialType material;
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

class sphere : public Shape
{
	double radius;
	point3 center;
	MaterialType materialType;
	color albedo;
	color emission;
public:
	sphere(const point3 center_, const double& r, color emission_, color albedo_, MaterialType material)
		: radius(r), center(center_), materialType(material), albedo(albedo_), emission(emission_) {}

	bool intersect(const Ray& ray, inform& _inform) const override
	{
		Vector3 oc = center - ray.origin;
		double neg_b = oc.Dot(ray.direction);
		double c = oc.Dot(oc) - radius * radius;
		double det = neg_b * neg_b - c;
		if (det < 0)
			return false;
		else
			det = sqrt(det);

		double epsilon = 1e-4;
		if (double t = neg_b - det; t > epsilon && t < _inform.distance)
		{
			_inform.distance = t;
			_inform.albedo = albedo;
			_inform.position = ray.origin + ray.direction * t;
			_inform.normal = _inform.position - center;
			set_normal(ray, _inform.normal.Normalize(), _inform.outface);
			_inform.emission = emission;
			_inform.material = materialType;
			return true;
		}
		else if (t = neg_b + det; t > epsilon && t < _inform.distance)
		{
			_inform.distance = t;
			_inform.albedo = albedo;
			_inform.position = ray.origin + ray.direction * t;
			_inform.normal = _inform.position - center;
			set_normal(ray, _inform.normal.Normalize(), _inform.outface);
			_inform.emission = emission;
			_inform.material = materialType;
			return true;
		}
		return false;
	}
};

class triangle : public Shape
{
	Vector3 p0, p1, p2;
	MaterialType materialType;
	color albedo;
	color emission;
public:
	triangle() = default;
	triangle(const Vector3& _p1, const Vector3& _p2, const Vector3& _p3, color emission_, color albedo_, MaterialType material)
		: p0(_p1), p1(_p2), p2(_p3), materialType(material), albedo(albedo_), emission(emission_) {}

	bool intersect(const Ray& ray, inform& _inform) const override
	{
		Vector3 E1 = p1 - p0;
		Vector3 E2 = p2 - p0;
		Vector3 P = ray.direction.Cross(E2);
		double det = E1.Dot(P);
		if (fabs(det) < 1e-5) return false;
		const double invS1E1 = 1. / det;
		Vector3 T = ray.origin - p0;
		double u = invS1E1 * T.Dot(P);
		if (u < 0 || u > 1) return false;

		Vector3 Q = T.Cross(E1);
		double v = ray.direction.Dot(Q) * invS1E1;
		if (v < 0 || u + v>1) return false;

		double t = E2.Dot(Q) * invS1E1;

		if (t < 0)
			return false;

		Vector3 v01 = p1 - p0;
		Vector3 v02 = p2 - p0;

		_inform.normal = v01.Cross(v02).Normalize();
		// if (_inform.normal.Dot(ray.direction) > 0)
			// _inform.normal = -_inform.normal;
		// _inform.outface = true;
		set_normal(ray, _inform.normal, _inform.outface);
		_inform.position = ray.origin + ray.direction * t;
		_inform.distance = t;
		_inform.albedo = albedo;
		_inform.emission = emission;
		_inform.material = materialType;
		return true;

	}
};

class quad : public Shape
{

};

class Shapes : public Shape
{
	std::vector<std::shared_ptr<Shape>> objects;
public:
	Shapes() = default;
	Shapes(const std::shared_ptr<Shape>& object) { objects.push_back(object); }
	void add(const std::shared_ptr<Shape>& object)
	{
		objects.push_back(object);
	}

	bool intersect(const Ray& ray, inform& _inform) const override
	{
		double infinity = 1e20;
		double minDistance = _inform.distance = infinity;

		for (const auto& object : objects)
		{
			if (object->intersect(ray, _inform))
			{
				if (_inform.distance < minDistance)
					minDistance = _inform.distance;
			}
		}

		return minDistance < infinity;
	}
};