#include <iostream>
#include <fstream>

#include <mpi.h>


#include "rtweekend.h"
#include "Renderer.h"



int main(int argc, char** argv)
{
	// MPI
	MPI_Init(&argc, &argv);

	int mode, rank;
	std::string filename;
	//mode = CHECKER_BOARDS;
	//mode = RANDOM_SPHERES;
	//mode = EARTH_SPHERE;
	//mode = PERLIN_SPHERE;
	mode = QUADS;
	
	//filename = "RT19.ppm";
	//filename = "RT20.ppm";
	//filename = "RT21.ppm";
	//filename = "RT22.ppm";
	filename = "RT23.ppm";

	renderer* render = new renderer(argc, argv, mode, filename);

	rank = render->para_ptr()->return_rank();

	camera_parallel* cam = render->cam_ptr();

	cam->move_camera(point3(0, 0, 12));

	render->setup();

	if (rank == 0)
	{
		std::clog << "Rendering                           " << std::endl;
		fflush(stdout);
	}

	render->render();

	if (rank == 0)
	{
		std::clog << "Gathering data from nodes           " << std::endl;
		fflush(stdout);
	}
	render->gather();
	
	if (rank == 0)
	{
		std::clog << "Writing the data              " << std::endl;
		fflush(stdout);
	}
	render->write_file();
	
	MPI_Finalize();
}