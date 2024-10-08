#ifndef CAMERA_PARALLEL_H
#define CAMERA_PARALLEL_H

#include "camera.h"

class camera_parallel : public camera
{
public:
	camera_parallel(const int _height_min, const int _height_max, const int _width_min, const int _width_max) :
		height_min(_height_min), height_max(_height_max), width_min(_width_min), width_max(_width_max) {}
	camera_parallel():height_min(0.0), height_max(0.0), width_min(0.0), width_max(0.0) {}
	void render(const hittable& world, rays_array& r_a)
	{
		initialize();
		for (int j = height_min; j < height_max; j++)
		{
			for (int i = width_min; i < width_max; i++)
			{
				color pixel_color(0, 0, 0);
				for (int sample = 0; sample < samples_per_pixel; sample++)
				{
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, max_depth, world);
				}
				rays_array_add(r_a, i-width_min, j-height_min, pixel_samples_scale * pixel_color);
			}
		}
	}
	void set_range(const int& _width_min, const int& _width_max, const int& _height_min, const int& _height_max)
	{
		this->width_min = _width_min;
		this->width_max = _width_max;
		this->height_min = _height_min;
		this->height_max = _height_max;
	}

private:
	int height_min, height_max, width_min, width_max;
};

#endif