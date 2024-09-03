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
#include "camera_parallel.h"


int main()
{
	// MPI
	MPI_Init(NULL, NULL);

	// World
	hittable_list world;

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
	camera_parallel cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1920;
	cam.file = &file;
	cam.samples_per_pixel = 500;
	cam.max_depth = 50;
	cam.initialize();

	// distribute load
	int size, rank;
	MPI_Comm MPI_world(MPI_COMM_WORLD);
	MPI_Comm_rank(MPI_world, &rank);
	MPI_Comm_size(MPI_world, &size);
	int width_per_node = int(cam.image_width / size + 1);
	int height_per_node = cam.image_height;
	int width_min = rank * width_per_node;
	int width_max = width_min + width_per_node;
	int height_min = 0;
	int height_max = cam.image_height;
	cam.set_range(width_min, width_max, height_min, height_max);

	// rays_array
	rays_array r_array(width_per_node, height_per_node);

	//cam.vfov = 90;
	cam.vfov = 20;
	cam.lookfrom = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = point3(0, 1, 0);


	cam.defocus_angle = 0.6;
	cam.focus_dist = 10.0;

	cam.render(world, r_array);


	// gather the data
	double* rs = r_array.rs[0];
	double* gs = r_array.gs[0];
	double* bs = r_array.bs[0];



	double* rs_all = (double*)malloc(width_per_node * height_per_node * size * sizeof(double));
	double* gs_all = (double*)malloc(width_per_node * height_per_node * size * sizeof(double));
	double* bs_all = (double*)malloc(width_per_node * height_per_node * size * sizeof(double));


	//
	MPI_Allgather(rs, width_per_node * height_per_node, MPI_DOUBLE, rs_all, width_per_node * height_per_node, MPI_DOUBLE, MPI_world);
	MPI_Allgather(gs, width_per_node * height_per_node, MPI_DOUBLE, gs_all, width_per_node * height_per_node, MPI_DOUBLE, MPI_world);
	MPI_Allgather(bs, width_per_node * height_per_node, MPI_DOUBLE, bs_all, width_per_node * height_per_node, MPI_DOUBLE, MPI_world);

	rays_array r_array_all(rs_all, gs_all, bs_all, cam.image_width, cam.image_height);

	// write class
	if (rank == 0) {
		write write_obj(&file, &r_array_all, cam.image_width, cam.image_height);
		write_obj.write_file();
	}

	MPI_Finalize();
}
