#pragma once
#include <QImage>
#include <random>
#include <algorithm>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>

#include "Camera.h"
#include "Vector.h"
#include "Shape.h"

QRgb Color2QRgb(color c)
{
	return qRgb(GammaEncoding(c.x), GammaEncoding(c.y), GammaEncoding(c.z));
}

class RNG
{
	std::random_device rd;
	std::mt19937_64 eng = std::mt19937_64(rd());
	std::uniform_real_distribution<double> dist = std::uniform_real_distribution<double>(0, 1);
public:
	RNG() = default;

	double random_double01()
	{
		return dist(eng);
	}

	Vector3 random_double01_vec3()
	{
		return { dist(eng), dist(eng), dist(eng) };
	}

};

class Sampler
{
protected:
	RNG rng_{};
	Camera cam_{};
	int samplesPerPixel{};
public:
	Sampler() = default;
	virtual ~Sampler() = default;
	Sampler(const int SPP, const Camera& cam) : cam_(cam), samplesPerPixel(SPP) {}
	int get_spp() const
	{
		return samplesPerPixel;
	}

	void set_spp(const int spp)
	{
		samplesPerPixel = spp;
	}

	Vector3 Radiance(const Ray& ray, const world& ws, int depth);
	void Renderer(QImage& image, const world& ws);

};

inline Vector3 Sampler::Radiance(const Ray& ray, const world& ws, int depth)
{
	if (inform inf; !ws.intersect(ray, inf))
	{
		return { 0, 0, 0 };
	}
	else
	{
		color inf_albedo = inf.material.get_albedo();
		double max_component = std::max({ inf_albedo.x, inf_albedo.y, inf_albedo.z });
		if (++depth > 5)
		{
			if (++depth > 5)
			{
				if (rng_.random_double01() < max_component)
					inf.material.set_albedo(inf.material.get_albedo() * (1 / max_component));
				else
					return inf.material.get_emission();
			}
		}
		if (depth > 10)
			return inf.material.get_emission();

		if (inf.material.get_mat_type() == material_type::diffuse)
		{
			double random1 = 2 * PI * rng_.random_double01();
			double random2 = rng_.random_double01();
			double random2Sqrt = sqrt(random2);
			//BRDF Render Equation
			Vector3 w = inf.normal;
			Vector3 u = ((fabs(w.x) > .1 ? Vector3(0, 1, 0) : Vector3(1, 0, 0)).Cross(w)).Normalize();
			Vector3 v = w.Cross(u);

			Vector3 wi_direction = (u * cos(random1) * random2Sqrt + v * sin(random1) * random2Sqrt + w * sqrt(1 - random2)).Normalize();

			double cos_theta_i = wi_direction.Dot(inf.normal);
			double Pdf = cos_theta_i / PI; // pdf = cos_theta / PI, diffuse BRDF = albedo / PI
			return inf.material.get_emission() + inf.material.get_albedo() * Radiance(Ray(inf.intersection, wi_direction), ws, depth);
		}
		else if (inf.material.get_mat_type() == material_type::specular)
		{
			Vector3 refl_dir = ray.direction - 2 * ray.direction.Dot(inf.normal) * inf.normal;
			//specular BRDF = albedo / cos_theta
			//pdf = 1
			//Lo = Le +  Li * cos_theta * albedo / cos_theta = Le + Li * albedo;

			return inf.material.get_emission() + inf.material.get_albedo() * Radiance(Ray(inf.intersection, refl_dir), ws, depth);
		}
		else
		{
			Vector3 refl_dir = ray.direction - 2 * ray.direction.Dot(inf.normal) * inf.normal;

			//snell`s law: sinI / sinT = theta_T / theta_I
			double theta_i = 1.0;
			double theta_t = 1.5;

			double eta_frac = inf.outface ? theta_i / theta_t : theta_t / theta_i;

			double cos_etaI = inf.normal.Dot(-ray.direction);
			double sin_etaT2 = std::max(0., 1. - cos_etaI * cos_etaI);
			sin_etaT2 *= eta_frac * eta_frac;
			if (sin_etaT2 >= 1)
				return inf.material.get_emission() + inf.material.get_albedo() * Radiance(Ray(inf.intersection, refl_dir), ws, depth);
			double cos_etaT = std::sqrt(1 - sin_etaT2);
			Vector3 refractDir = ray.direction * eta_frac + (eta_frac * cos_etaI - cos_etaT) * inf.normal;
			// double cos_etaT2 = 1 - eta_frac * eta_frac * (1 - cos_eta * cos_eta);
			// if (cos_etaT2 < 0)
			//     return inf.emission + inf.albedo * Radiance(Ray(inf.position, refl_dir), s, depth);
			// double cos_etaT = sqrt(cos_etaT2);
			// Vector3 refractDir = (ray.direction * eta_frac + inf.normal * (eta_frac * cos_etaI - cos_etaT)).Normalize();

			double R0 = (theta_t - theta_i) / (theta_t + theta_i);
			R0 *= R0;

			double R_theta = R0 + (1 - R0) * pow(1 - (inf.outface ? cos_etaI : cos_etaT), 5);
			double Tr = 1 - R_theta;

			double P = .25 + .5 * R_theta;
			double RP = R_theta / P;
			double TP = Tr / (1 - P);

			color Li;
			if (depth > 2)
			{
				if (rng_.random_double01() < P)
				{
					Li = Radiance(Ray(inf.intersection, refl_dir), ws, depth) * RP;
				}
				else
				{
					Li = Radiance(Ray(inf.intersection, refractDir), ws, depth) * TP;
				}
			}
			else
			{
				Li = Radiance(Ray(inf.intersection, refl_dir), ws, depth) * R_theta + Radiance(Ray(inf.intersection, refractDir), ws, depth) * Tr;
			}
			return inf.material.get_emission() + inf.material.get_albedo() * Li;
		}
	}
}

inline void Sampler::Renderer(QImage& image, const world& ws)
{
	tbb::blocked_range2d<int>range2d(0, cam_.get_res().x, 1, 0, cam_.get_res().y, 1);
	tbb::parallel_for(range2d, [&](const tbb::blocked_range2d<int>& r)
		{
			for (int y = r.cols().begin(); y < r.cols().end(); y++)
			{
				// fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samplesPerPixel * 4, 100. * y / (cam_.get_res().y - 1));
				// std::cerr << "\rRendering (" << SPP * 4 << " spp) " << std::setw(5) << 100. * y / (image_height - 1) << "%";
				for (int x = r.rows().begin(); x < r.rows().end(); x++)
				{
					// std::cout << y << x << std::endl;
					QRgb pixel = qRgb(0, 0, 0);
					color sum;
					color Li;
					for (int sx = 0; sx < 2; sx++)
					{
						for (int sy = 0; sy < 2; sy++)
						{
							for (int k = 0; k < samplesPerPixel; k++)
							{
								double random1 = 2 * rng_.random_double01();
								double random2 = 2 * rng_.random_double01();
								double dx = random1 < 1 ? sqrt(random1) - 1 : 1 - sqrt(2 - random1);
								double dy = random2 < 1 ? sqrt(random2) - 1 : 1 - sqrt(2 - random2);

								Ray ray = cam_.GenerateRay(x, y, dx, dy);
								Li = Li + Radiance(ray, ws, 0) * 1. / samplesPerPixel;
							}
							sum = sum + color(clamp(Li.x), clamp(Li.y), clamp(Li.z)) * 0.25;
						}
					}
					pixel = Color2QRgb(sum);
					image.setPixel(x, y, pixel);
				}
			}
		}
	);
}
