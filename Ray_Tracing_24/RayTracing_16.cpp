#include <iostream>
#include <fstream>

#include <mpi.h>


#include "rtweekend.h"

#include "aabb.h"
#include "bvh.h"
#include "camera.h"
#include "camera_parallel.h"
#include "color.h"
#include "color_array.h"
#include "hittable.h"
#include "hittable_list.h"
#include "input.h"
#include "interval.h"
#include "material.h"
#include "parallel.h"
#include "ray.h"
#include "sphere.h"
#include "vec3.h"
#include "write.h"


int main(int argc, char **argv)
{
	
	// MPI
	MPI_Init(&argc,&argv);

	// Parsing input arguments
	input in(argc, argv);


	// World
	hittable_list world, *world_ptr;
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
					auto center2 = center + vec3(0, random_double(0, 0.5), 0);
					world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
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

	world = hittable_list(make_shared<bvh_node>(world));


	// Camera
	camera_parallel cam, * cam_ptr;
	cam_ptr = &cam;

	in.setup_camera(cam_ptr);



	parallel para(cam_ptr, world_ptr);
	color_array* c_array_ptr = para.color_array_ptr();
	color_array* c_array_all_ptr;


	// knowing the rank of a process is required for priting the output
	int rank = para.return_rank();
	
	std::string file_name = "RT16.ppm";
	std::ofstream file(file_name);

	cam_ptr->move_camera(point3(13, 2, 3));

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
		std::clog << "Writing the data              " << std::endl;
		int image_width, image_height;
		cam.return_image_size(image_width, image_height);
		write write_obj(&file, c_array_all_ptr, image_width, image_height);
		write_obj.write_file();
	}


	if (rank == 0)  
	{
		std::clog << "Cleanning up                    " << std::endl;
		fflush(stdout);
	}

	MPI_Finalize();
}