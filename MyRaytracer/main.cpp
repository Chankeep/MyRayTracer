#include <iostream>
#include <iomanip>
#include "Vector.h"
#include "Shape.h"
#include "Camera.h"
#include "RND.h"

#include <tbb/tbb.h>

#include <QtWidgets/QApplication>
#include <QImage>
#include "MyRaytracer.h"

int image_width = 1000;
int image_height = 1200;

RND random;

QRgb Color2QRgb(color c)
{
	return qRgb(GammaEncoding(c.x), GammaEncoding(c.y), GammaEncoding(c.z));
}

Vector3 Radiance(const Ray& ray, const Shapes& s, int depth)
{
	inform inf;
	if (!s.intersect(ray, inf))
	{
		return color(0, 0, 0);
	}
	else
	{
		double max_component = std::max({ inf.albedo.x, inf.albedo.y, inf.albedo.z });
		if (++depth > 5)
		{
			if (++depth > 5)
			{
				if (random.random_double01() < max_component)
					inf.albedo = inf.albedo * (1 / max_component);
				else
					return inf.emission;
			}
		}
		if (depth > 10)
			return inf.emission;

		if (inf.material == MaterialType::diffuse)
		{
			double random1 = 2 * PI * random.random_double01();
			double random2 = random.random_double01();
			double random2Sqrt = sqrt(random2);
			//BRDF Render Equation
			Vector3 w = inf.normal;
			Vector3 u = ((fabs(w.x) > .1 ? Vector3(0, 1, 0) : Vector3(1, 0, 0)).Cross(w)).Normalize();
			Vector3 v = w.Cross(u);

			Vector3 wi_direction = (u * cos(random1) * random2Sqrt + v * sin(random1) * random2Sqrt + w * sqrt(1 - random2)).Normalize();

			double cos_theta_i = wi_direction.Dot(inf.normal);
			double Pdf = cos_theta_i / PI; // pdf = cos_theta / PI, diffuse BRDF = albedo / PI, 渲染方程里面还需乘一个cos_theta，故全抵消，也实现了重要性采样
			return inf.emission + inf.albedo * Radiance(Ray(inf.position, wi_direction), s, depth);
		}
		else if (inf.material == MaterialType::specular)
		{
			Vector3 refl_dir = ray.direction - 2 * ray.direction.Dot(inf.normal) * inf.normal;
			//specular BRDF = albedo / cos_theta
			//pdf = 1
			//Lo = Le +  Li * cos_theta * albedo / cos_theta = Le + Li * albedo;

			return inf.emission + inf.albedo * Radiance(Ray(inf.position, refl_dir), s, depth);
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
				return inf.emission + inf.albedo * Radiance(Ray(inf.position, refl_dir), s, depth);
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
				if (random.random_double01() < P)
				{
					Li = Radiance(Ray(inf.position, refl_dir), s, depth) * RP;
				}
				else
				{
					Li = Radiance(Ray(inf.position, refractDir), s, depth) * TP;
				}
			}
			else
			{
				Li = Radiance(Ray(inf.position, refl_dir), s, depth) * R_theta + Radiance(Ray(inf.position, refractDir), s, depth) * Tr;
			}
			return inf.emission + inf.albedo * Li;
		}
	}


}

int main(int argc, char* argv[])
{
	std::cout << "请输入每个像素发射光线根数：（推荐50根）";
	int SPP=1;
	std::cin >> SPP;
	std::cout << "请输入渲染图像分辨率：（x）";
	std::cin >> image_width;
	std::cout << "请输入渲染图像分辨率：（y）";
	std::cin >> image_height;
	std::cout << "渲染中，耐心等待" << std::endl;
	QImage image(image_width, image_height, QImage::Format_ARGB32);
	// QImage test(800, 800, QImage::Format_ARGB32);
	QString filename = "render_image.jpg";
	QApplication a(argc, argv);
	MyRaytracer w;
	// w.show();

	Camera cam(point3(0, 16, 35), point3(0, 0, -1).Normalize(), 40, image_width,
		image_height);

	triangle tri2(point3(-18, 30, -44.5), point3(18, 30, -44.5), point3(18, 0, -44.5), color(0, 0, 0), color(1, 1, 1), MaterialType::diffuse);
	triangle tri1(point3(-18, 30, -44.5), point3(18, 0, -44.5), point3(-18, 0, -44.5), color(0, 0, 0), color(1, 1, 1), MaterialType::diffuse);
	sphere sph2(point3(-10.5, 5, -20), 5, color(), color(1., 1., 1.), MaterialType::Refract);
	sphere sph1(point3(8, 8, -20), 5, color(), color(0.6, 0.6, 0.6), MaterialType::specular);
	sphere ground(point3(0, -10000, -20), 10000, color(), color(0.6, 0.5, 0.6), MaterialType::diffuse);
	sphere light(point3(0, 629.96, -20), 600, color(12, 12, 12), color(), MaterialType::diffuse);
	sphere left(point3(-10018, 0, -20), 10000, color(), color(0.25, 0.25, 0.75), MaterialType::diffuse);
	sphere right(point3(10018, 0, -20), 10000, color(), color(0.75, 0.25, 0.25), MaterialType::diffuse);
	sphere top(point3(0, 10030, -20), 10000, color(), color(0.25, 0.75, 0.25), MaterialType::diffuse);
	sphere front(point3(0, 0, -10045), 10000, color(), color(0.75, 0.75, 0.75), MaterialType::diffuse);
	sphere back(point3(0, 0, 10036), 10000, color(), color(0.25, 0.25, 0.75), MaterialType::diffuse);

	Shapes shapes;
	// shapes.add(std::make_shared<triangle>(tri1));
	shapes.add(std::make_shared<sphere>(front));
	shapes.add(std::make_shared<sphere>(sph2));
	shapes.add(std::make_shared<sphere>(ground));
	shapes.add(std::make_shared<sphere>(light));
	shapes.add(std::make_shared<sphere>(top));
	shapes.add(std::make_shared<sphere>(left));
	shapes.add(std::make_shared<sphere>(right));
	// shapes.add(std::make_shared<triangle>(tri2));
	shapes.add(std::make_shared<sphere>(back));
	shapes.add(std::make_shared<sphere>(sph1));

	tbb::blocked_range2d<int>range2d(0, image_width, 1, 0, image_height, 1);
	tbb::parallel_for(range2d, [&](const tbb::blocked_range2d<int>& r)
		{
			for (int y = r.cols().begin(); y < r.cols().end(); y++)
			{
				// fprintf(stderr, "\rRendering (%d spp) %5.2f%%", SPP * 4, 100. * y / (image_height - 1));
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
							for (int k = 0; k < SPP; k++)
							{
								double random1 = 2 * random.random_double01();
								double random2 = 2 * random.random_double01();
								double dx = random1 < 1 ? sqrt(random1) - 1 : 1 - sqrt(2 - random1);
								double dy = random2 < 1 ? sqrt(random2) - 1 : 1 - sqrt(2 - random2);

								Ray ray = cam.GenerateRay(x, y, dx, dy);
								Li = Li + Radiance(ray, shapes, 0) * 1. / SPP;
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
	std::cout << std::endl;
	if (!image.save(filename))
	{
		std::cout << "image save error!" << std::endl;
	}
	// test.save(filename);
	// return QApplication::exec();



	return 0;
}
