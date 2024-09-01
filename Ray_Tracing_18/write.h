#ifndef WRITE_H
#define WRITE_H

#include <fstream>
#include "rays_array.h"

class write {
public:
	write(std::ofstream* _file, rays_array* _r_array, int _image_width, int _image_height): file(_file), r_array(_r_array), image_width(_image_width), image_height(_image_height) {}
	void write_file()
	{
		*file << "P3\n" << image_width << " " << image_height << "\n255\n";

		for (int j = 0; j < image_height; j++)
		{
			for (int i = 0; i < image_width; i++)
			{
				double r = r_array->rs[i][j];
				double g = r_array->gs[i][j];
				double b = r_array->bs[i][j];
				color color_ij(r, g, b);
				write_color(*file, color_ij);
			}
		}
	}
	~write()
	{
		file->close();
	}
private:
	std::ofstream* file;
	rays_array* r_array;
	int image_width, image_height;
	vec3** data;
};




#endif