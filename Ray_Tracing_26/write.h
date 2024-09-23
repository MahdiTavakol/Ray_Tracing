#ifndef WRITE_H
#define WRITE_H

#include <fstream>

#include "rtweekend.h"

#include "color_array.h"

class write {
public:
	write(std::ofstream* _file, color_array* _colors, int _image_width, int _image_height);
	write(std::ofstream* _file);
	~write();
	void write_file();
	void reset(color_array*_colors, const int _image_width, const int _image_height);
private:
	std::ofstream* file;
	color_array* colors;
	int image_width, image_height;
};




#endif