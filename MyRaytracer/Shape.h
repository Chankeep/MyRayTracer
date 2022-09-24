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
	virtual bool intersect(const Ray& ray, inform &_inform) const = 0;
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

	static void set_normal(const Ray& ray, Vector3& normal, bool &outface)
	{
		outface = ray.direction.Dot(normal) < 0;
		normal = outface ? normal : -normal;
	}

	bool intersect(const Ray& ray, inform &_inform) const override
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
		if(double t = neg_b - det; t > epsilon && t < _inform.distance)
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
		else if(t = neg_b + det; t > epsilon && t < _inform.distance)
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

	bool intersect(const Ray& ray, inform &_inform) const override
	{
		double infinity = 1e20;
		double minDistance = _inform.distance = infinity;

		for(int i=0; i<objects.size(); i++)
		{
			if(objects[i]->intersect(ray, _inform))
			{
				if(_inform.distance < minDistance)
					minDistance = _inform.distance;
			}
		}

		return minDistance < infinity;
	}
};