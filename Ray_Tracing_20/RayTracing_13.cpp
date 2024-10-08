#include <iostream>
#include <fstream>

#include <mpi.h>


#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "write.h"
#include "color_array.h"
#include "camera_parallel.h"
#include "parallel.h"


int main()
{
	// MPI
	MPI_Init(NULL, NULL);

	// World
	hittable_list world, *world_ptr; // I am not sure if the following lines do not change the address of world
	world_ptr = &world;

	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++)
		for (int b = -11; b < 11; b++)
		{
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}

		}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));



	// Output file
	std::ofstream file("RT20.ppm");


	// Camera
	camera_parallel cam, * cam_ptr;
	cam_ptr = &cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1920;
	cam.samples_per_pixel = 500;
	cam.max_depth = 50;
	cam.initialize();

	//cam.vfov = 90;
	cam.vfov = 20;
	cam.lookfrom = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = point3(0, 1, 0);


	cam.defocus_angle = 0.6;
	cam.focus_dist = 10.0;


	parallel para(cam_ptr,world_ptr);
	color_array* c_array_ptr = para.color_array_ptr();
	color_array* c_array_all_ptr;
	int rank;
	rank = para.return_rank();
	if (rank == 0)
	{
		std::clog << "Rendering                           " << std::endl;
		fflush(stdout);
	}
	para.render();
	if (rank == 0)
	{
		std::clog << "Gathering data from nodes           " << std::endl;
		fflush(stdout);
	}
	para.gather();
	c_array_all_ptr = para.color_array_all_ptr();




	// write class
	if (rank == 0) {
		write write_obj(&file, c_array_all_ptr, cam.image_width, cam.image_height);
		write_obj.write_file();
	}
	if (rank == 0)  
	{
		std::clog << "Cleanning up                    " << std::endl;
		fflush(stdout);
	}
	MPI_Finalize();
}
