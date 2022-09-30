#pragma once
#include "Vector.h"

enum class material_type { diffuse, specular, refract };

class Material
{
	color emission_;
	color albedo_;
	material_type mat_type_{material_type::diffuse};
public:
	Material() = default;
	Material(const color emission, const color albedo)
		: emission_(emission), albedo_(albedo) {}
	Material(const color emission, const color albedo, const material_type material)
		: emission_(emission), albedo_(albedo), mat_type_(material) {}

	color get_emission() const;
	void set_emission(const color&);
	color get_albedo() const;
	void set_albedo(const color&);
	material_type get_mat_type() const;
	void set_mat_type(const material_type&);
};

inline color Material::get_emission() const
{
	return emission_;
}

inline void Material::set_emission(const color& emission)
{
	emission_ = emission;
}

inline color Material::get_albedo() const
{
	return albedo_;
}

inline void Material::set_albedo(const color& albedo)
{
	albedo_ = albedo;
}

inline material_type Material::get_mat_type() const
{
	return mat_type_;
}

inline void Material::set_mat_type(const material_type& mat_type)
{
	mat_type_ = mat_type;
}
