#ifndef COLOR_ARRAY_H
#define COLOR_ARRAY_H

#include <stdlib.h>

typedef struct { double r, g, b; } color_data;

class color_array
{
public:
	color_array(const int& _width, const int& _height): width(_width), height(_height) 
	{
		allocate();
	}
	color_array(const int& _width, const int& _height, color_data* _color_data): color_array(_width, _height)
	{
		allocate();
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				this->array[i][j] = _color_data[i * height + j]; //1D to 2D array conversion
	}
	color_array(const int& _width, const int& _height, color_data** _color_data) : color_array(_width, _height)
	{
		allocate();
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				this->array[i][j] = _color_data[i][j];
	}
	color_array():color_array(0.0, 0.0) {}
	~color_array()
	{
		deallocate();
	}
	void set_range(const int& _width, const int& _height)
	{
		this->width = _width;
		this->height = _height;
	}

	color_array& operator=(color_array second) {
		if (this == &second)
		{
			return *this;
		}
		this->deallocate();
		this->width = second.width;
		this->height = second.height;
		this->allocate();
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
			{
				this->array[i][j].r = second.array[i][j].r;
				this->array[i][j].g = second.array[i][j].g;
				this->array[i][j].b = second.array[i][j].b;
			}
		return *this;
	}

	color_data**& return_array()
	{
		return this->array;
	}



private:
	int width, height;
	color_data** array;

	void allocate()
	{
		color_data* temp = (color_data*)malloc(width * height * sizeof(color_data));
		this->array = (color_data**)malloc(width * sizeof(color_data*)); // Column major allocation since we use color_data[x_index][y_index]
		for (int i = 0; i < width; i++)
			this->array[i] = &temp[i * height];
	}
	void deallocate()
	{
		free(this->array[0]);
		free(this->array);
	}
};


inline std::ostream& operator<<(std::ostream& out, const color_data& _c_data)
{
	static const interval intensity(0.000, 0.999);
	double r = _c_data.r;
	double g = _c_data.g;
	double b = _c_data.b;
	int rbyte = int(256 * intensity.clamp(r));
	int gbyte = int(256 * intensity.clamp(g));
	int bbyte = int(256 * intensity.clamp(b));
	return out << rbyte << " " << gbyte << " " << bbyte << std::endl;
}



#endif