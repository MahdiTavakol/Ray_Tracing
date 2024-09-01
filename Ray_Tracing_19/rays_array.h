#ifndef RAYS_ARRAY_H
#define RAYS_ARRAY_H

#include <iostream>
#include <mpi.h>

class rays_array
{
public:
	rays_array(const int& _num_pixels_x, const int& _num_pixels_y) : num_pixels_x(_num_pixels_x), num_pixels_y(_num_pixels_y)
	{
		allocate_array(rs, num_pixels_x, num_pixels_y);
		allocate_array(gs, num_pixels_x, num_pixels_y);
		allocate_array(bs, num_pixels_x, num_pixels_y);
	}
	rays_array(double* _rs, double* _gs, double* _bs, int _width, int _height) :num_pixels_x(_width), num_pixels_y(_height)
	{
		this->rs = (double **)_rs;
		this->gs = (double **)_gs;
		this->bs = (double **)_bs;
	}
	~rays_array()
	{
		deallocate_array(rs);
		deallocate_array(gs);
		deallocate_array(bs);
	}

	double** rs, ** gs, **bs;
	int num_pixels_x, num_pixels_y;

private:
	void allocate_array(double**& array, const int& num_x, const int& num_y)
	{
		double* temp = (double*)malloc(num_x * num_y * sizeof(double));
		array = (double**)malloc(num_x * sizeof(double*));

		for (int i = 0; i < num_x; i++)
			array[i] = &temp[num_y * i];
	}

	void deallocate_array(double**& array)
	{
		free(array[0]);
		free(array);
		array = nullptr;
	}
	
};

void rays_array_add(rays_array& _rays_array, const int& i, const int& j, const color& _color)
{
	if (i >= _rays_array.num_pixels_x ||
		j >= _rays_array.num_pixels_y ||
		i < 0 ||
		j < 0) 
	{
		int rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		std::cout << "Out of boundary! from rank " << rank << " for " << i << "," << j << std::endl;
		return;
	}
	_rays_array.rs[i][j] = _color.x();
	_rays_array.gs[i][j] = _color.y();
	_rays_array.bs[i][j] = _color.z();
}




#endif