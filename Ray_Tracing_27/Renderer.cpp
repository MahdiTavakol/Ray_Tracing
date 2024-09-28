#include "Renderer.h"



renderer::renderer(int argc, char** argv, int _mode, std::string _filename) 
	: mode(_mode), c_array(nullptr), c_array_all(nullptr)
{
	in = new input(argc, argv,mode);
	cam = new camera_parallel(in);
	world = new hittable_list();
	para = new parallel(cam, world);
	file = new std::ofstream(_filename);
	writer = new write(file);
}

renderer::~renderer()
{
	delete in;
	delete cam;
	delete world;
	delete para;
	delete file;
	delete writer;
	c_array = nullptr;
	c_array_all = nullptr;
}

void renderer::setup()
{
	c_array = para->color_array_ptr();

	switch (mode)
	{
	case RANDOM_SPHERES:
		setup_random_spheres();
		break;
	case CHECKER_BOARDS:
		setup_checker_boards();
		break;
	case EARTH_SPHERE:
		setup_earth_sphere();
		break;
	case PERLIN_SPHERE:
		setup_perlin_sphere();
		break;
	case QUADS:
		setup_quads();
		break;
	}

	para->setup(cam, world);

}


void renderer::add(shared_ptr<hittable> object)
{
	world->add(object);
}

void renderer::render()
{
	c_array = para->color_array_ptr(); // This points to the color_array object belonging to the parallel object.
	para->render();
}

void renderer::gather()
{
	para->gather();
	c_array_all = para->color_array_all_ptr();
}

void renderer::write_file()
{
	int image_width, image_height;
	cam->return_image_size(image_width, image_height);
	writer->reset(c_array_all, image_width, image_height);

	int rank = para->return_rank();
	if (rank == 0) writer->write_file();
}

parallel* renderer::para_ptr() const {
	return para;
}

camera_parallel* renderer::cam_ptr() const
{
	return cam;
}

void renderer::setup_random_spheres()
{
	auto checker = make_shared<checker_texture>(0.32, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	world->add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

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
					world->add(make_shared<sphere>(center, center2, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world->add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world->add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}

		}

	auto material1 = make_shared<dielectric>(1.5);
	world->add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world->add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world->add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
	
	*world = hittable_list(make_shared<bvh_node>(*world));
}

void renderer::setup_checker_boards()
{
	auto checker = make_shared<checker_texture>(0.32, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

	world->add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
	world->add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));
	
	*world = hittable_list(make_shared<bvh_node>(*world));
}

void renderer::setup_earth_sphere()
{
	auto earth_texture = make_shared<image_texture>("3840px-Blue_Marble_2002.png");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

	world->add(globe);

	*world = hittable_list(make_shared<bvh_node>(*world));
}

void renderer::setup_perlin_sphere()
{
	auto pertext = make_shared<noise_texture>(4);
	world->add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	world->add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

	*world = hittable_list(make_shared<bvh_node>(*world));

}

void renderer::setup_quads()
{
	auto left_red = make_shared<lambertian>(color(1.0, 0.2, 0.2));
	auto back_green = make_shared<lambertian>(color(0.2, 1.0, 0.2));
	auto right_blue = make_shared<lambertian>(color(0.2, 0.2, 1.0));
	auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
	auto lower_teal = make_shared<lambertian>(color(0.2, 0.8, 0.8));

	world->add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
	world->add(make_shared<circle>(point3(-2, -2, 0), vec3(4, 0,  0), vec3(0, 4, 0), back_green));
	world->add(make_shared<triangle>(point3( 3, -2, 1), vec3(0, 0,  4), vec3(0, 4, 0), right_blue));
	world->add(make_shared<quad>(point3(-2,  3, 1), vec3(4, 0,  0), vec3(0, 0, 4), upper_orange));
	world->add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0,  0), vec3(0, 0,-4), lower_teal));



}
