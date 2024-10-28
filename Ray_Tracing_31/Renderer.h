#ifndef RENDERER_H
#define RENDERER_H

#include "rtweekend.h"

#include "aabb.h"
#include "bvh.h"
#include "camera.h"
#include "camera_parallel.h"
#include "color.h"
#include "color_array.h"
#include "constant_medium.h"
#include "hittable.h"
#include "hittable_list.h"
#include "input.h"
#include "interval.h"
#include "material.h"
#include "parallel.h"
#include "quad.h"
#include "ray.h"
#include "rtw_stb_image.h"
#include "planes.h"
#include "perlin.h"
#include "sphere.h"
#include "texture.h"
#include "vec3.h"
#include "write.h"


class renderer
{
public:
	renderer(int argc, char** argv, int _mode, std::string _filename);
	~renderer();
	void setup();
	void add(shared_ptr<hittable> object);
	void render();
	void gather();
	void write_file();
	parallel* para_ptr() const;
	camera_parallel* cam_ptr() const;

private:
	int mode;
	camera_parallel* cam;
	input* in;
	hittable_list* world;
	parallel* para;
	color_array* c_array, * c_array_all;
	std::ofstream* file;
	write* writer;


	void setup_random_spheres();
	void setup_checker_boards();
	void setup_earth_sphere();
	void setup_perlin_sphere();
	void setup_quads();
	void setup_simple_light();
	void setup_two_lights();
	void setup_cornell_box();
	void setup_boxes();
	void setup_boxes_rotated();
	void setup_cornell_smoke();

};

#endif