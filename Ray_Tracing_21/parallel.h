#ifndef PARALLEL_H
#define PARALLEL_H

#include <iostream>

#include <mpi.h>
#include "rtweekend.h"

class camera_parallel;
class hittable_list;

class parallel
{
public:
	parallel(camera_parallel* _cam, hittable_list* _world): cam(_cam), world(_world)
	{
		// MPI variables
		MPI_world = MPI_COMM_WORLD;
		MPI_Comm_rank(MPI_world, &rank);
		MPI_Comm_size(MPI_world, &size);
		// distribute load
		width_per_node = int(cam->image_width / size + 1);
		height_per_node = cam->image_height;
		width_min = rank * width_per_node;
		width_max = width_min + width_per_node;
		height_min = 0;
		height_max = cam->image_height;
		cam->set_range(width_min, width_max, height_min, height_max);

		// colors_array
		c_array.reset_size(width_per_node, height_per_node);
		if (rank == 0)
		{
			std::clog << "An instance of the parallel class has been created" << std::endl;
			fflush(stdout);
		}
	}

	color_array* const color_array_ptr()
	{
		return &c_array;
	}
	color_array* const color_array_all_ptr()
	{
		return &c_array_all;
	}

	void render()
	{
		cam->render(*world, c_array);
	}

	void gather()
	{
		color_data* colors = c_array.return_array()[0];
		color_data* colors_all = (color_data*)malloc(width_per_node * height_per_node * size * sizeof(color_data));



		int num_double_data = width_per_node * height_per_node * sizeof(color_data) / sizeof(double);

		MPI_Allgather(colors, num_double_data, MPI_DOUBLE, colors_all, num_double_data, MPI_DOUBLE, MPI_world);

		c_array_all = color_array(cam->image_width, cam->image_height, colors_all);
	}

	int return_rank() const
	{
		return rank;
	}



protected:
	int size, rank;
	int width_per_node, height_per_node;
	int width_min, width_max, height_min, height_max;
	camera_parallel* cam;
	hittable_list* world;
	MPI_Comm MPI_world;
	color_array c_array, c_array_all;
};

#endif
