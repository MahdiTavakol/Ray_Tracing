#include <iostream>
#include <fstream>

#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"


color ray_color(const ray& r);
color ray_color(const ray& r, const hittable& world);
double hit_sphere(const point3& center, double radius, const ray& r);


int main()
{
	// World
	hittable_list world;

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<dielectric>(1.0/1.33);
	auto material_right = make_shared < metal>(color(0.8, 0.6, 0.2),1.0);

	world.add(make_shared<sphere>(point3(0.0,-100.5,-1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));


	// Output file
	std::ofstream file("RT12.ppm");

	// Camera
	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1080;
	cam.file = &file;
	cam.samples_per_pixel = 10;
	cam.max_depth = 50;

	cam.render(world);
}

color ray_color(const ray& r, const hittable& world)
{
	hit_record rec;
	if (world.hit(r, interval(0, infinity), rec))
	{
		return 0.5 * (rec.normal + color(1, 1, 1));
	}

	vec3 unit_direction = unit_vector(r.direction());
	auto a = 0.5 * (unit_direction.y() + 1.0);;
	return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

color ray_color(const ray& r) {
	auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
	if (t > 0.0)
	{
		vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
		return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
	}

	vec3 unit_direction = unit_vector(r.direction());
	auto a = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

double hit_sphere(const point3& center, double radius, const ray& r)
{
	vec3 oc = center - r.origin();
	auto a = r.direction().length_squared();
	auto h = dot(r.direction(), oc);
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = h*h - a*c;
	if (discriminant < 0)
	{
		return -1.0;
	}
	else
	{
		return (-h - std::sqrt(discriminant) / a);
	}
}