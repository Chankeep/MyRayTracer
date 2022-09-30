#include <iostream>
#include <iomanip>

#include "Sampler.h"

#include <QtWidgets/QApplication>
#include "MyRaytracer.h"

int image_width = 1000;
int image_height = 1200;

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

	//camera
	Camera cam(Transform(point3(0, 16, 35)), point3(0, 0, -1).Normalize(), 40, Vector2i(image_width,image_height));

	// triangle tri2(point3(-18, 30, -44.5), point3(18, 30, -44.5), point3(18, 0, -44.5), color(0, 0, 0), color(1, 1, 1), material_type::diffuse);
	// triangle tri1(point3(-18, 30, -44.5), point3(18, 0, -44.5), point3(-18, 0, -44.5), color(0, 0, 0), color(1, 1, 1), material_type::diffuse);
	sphere sph2(Transform(point3(-10.5, 5, -20)), 5, Material(color(), color(1., 1., 1.), material_type::refract));
	sphere sph1(Transform(point3(8, 8, -20)), 5, Material(color(), color(0.6, 0.6, 0.6), material_type::specular));
	sphere ground(Transform(point3(0, -10000, -20)), 10000, Material(color(), color(0.6, 0.5, 0.6), material_type::diffuse));
	sphere light(Transform(point3(0, 629.96, -20)), 600, Material(color(12, 12, 12), color(), material_type::diffuse));
	sphere left(Transform(point3(-10018, 0, -20)), 10000, Material(color(), color(0.25, 0.25, 0.75), material_type::diffuse));
	sphere right(Transform(point3(10018, 0, -20)), 10000, Material(color(), color(0.75, 0.25, 0.25), material_type::diffuse));
	sphere top(Transform(point3(0, 10030, -20)), 10000, Material(color(), color(0.25, 0.75, 0.25), material_type::diffuse));
	sphere front(Transform(point3(0, 0, -10045)), 10000, Material(color(), color(0.75, 0.75, 0.75), material_type::diffuse));
	sphere back(Transform(point3(0, 0, 10036)), 10000, Material(color(), color(0.25, 0.25, 0.75), material_type::diffuse));

	//worldScene
	world worldSpace;
	// shapes.add(std::make_shared<triangle>(tri1));
	worldSpace.add(std::make_shared<sphere>(front));
	worldSpace.add(std::make_shared<sphere>(sph2));
	worldSpace.add(std::make_shared<sphere>(ground));
	worldSpace.add(std::make_shared<sphere>(light));
	worldSpace.add(std::make_shared<sphere>(top));
	worldSpace.add(std::make_shared<sphere>(left));
	worldSpace.add(std::make_shared<sphere>(right));
	// shapes.add(std::make_shared<triangle>(tri2));
	worldSpace.add(std::make_shared<sphere>(back));
	worldSpace.add(std::make_shared<sphere>(sph1));

	//sampler
	Sampler MC(SPP, cam);
	MC.Renderer(image, worldSpace);

	std::cout << std::endl;
	if (!image.save(filename))
	{
		std::cout << "image save error!" << std::endl;
	}

	// return QApplication::exec();
	return 0;
}
