#include "global.h"
#include"texture.h"
#include "scene.h"
#include "object.h"
#include "objectlist.h"
#include "material.h"
#include "sphere.h"
#include"constant_medium.h"
#include"quad.h"
void earth(const char* filename)
{
	auto earth_texture = make_shared<Texture_color>("map.jpg");
	auto earth_surface = make_shared<diffuse>(earth_texture);
	auto textured_sphere = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);
	Scene cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.img_width = 400;
	cam.samples_per_pixel = 50;
	cam.max_depth = 50;
	cam.fov = 20;
	cam.viewport_distance = 12;
	cam.lookfrom = point3(0, 0, 12);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);
	cam.depth_field_angle = 0;
	cam.backgroundcolor = color(0.70, 0.80, 1.00);
	objectlist objects;
	objects.add(textured_sphere);
	objects = objectlist(make_shared<bvh_node>(objects));
	cam.render(objects, filename);
}
void quads(const char* filename) {
	objectlist world;
	// Materials
	auto left_red = make_shared<diffuse>(color(1.0, 0.2, 0.2));
	auto back_green = make_shared<diffuse>(color(0.2, 1.0, 0.2));
	auto right_blue = make_shared<diffuse>(color(0.2, 0.2, 1.0));
	auto upper_orange = make_shared<diffuse>(color(1.0, 0.5, 0.0));
	auto lower_teal = make_shared<diffuse>(color(0.2, 0.8, 0.8));
	// Quads
	world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
	world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
	world.add(make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
	world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
	world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));
	world = objectlist(make_shared<bvh_node>(world));
	Scene cam;
	cam.aspect_ratio = 1.0;
	cam.img_width = 400;
	cam.samples_per_pixel = 50;
	cam.max_depth = 20;
	cam.fov = 80;
	cam.lookfrom = point3(0, 0, 9);
	cam.lookat = point3(0, 0, 0);
	cam.viewport_distance = 9;
	cam.vup = vec3(0, 1, 0);
	cam.depth_field_angle = 0;
	cam.backgroundcolor = color(0.70, 0.80, 1.00);
	cam.render(world, filename);
}
void simple_light(const char*filename) {
	objectlist world;
	//auto pertext = make_shared<noise_texture>(4);
	auto earth_texture = make_shared<Texture_color>("map.jpg");
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<diffuse>(earth_texture)));
	world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<diffuse>(earth_texture)));
	auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
	world.add(make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), difflight));
	world.add(make_shared<sphere>(point3(0, 7, 0), 2, difflight));
	Scene cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.img_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;
	cam.viewport_distance = 10;
	cam.backgroundcolor = color(0, 0, 0);
	cam.fov = 20;
	cam.lookfrom = point3(26, 3, 6);
	cam.lookat = point3(0, 2, 0);
	cam.vup = vec3(0, 1, 0);
	cam.depth_field_angle = 0;
	cam.render(world, filename);
}
void cornell_box(const char*filename) {
	objectlist world;
	//Cornell box
	auto red = make_shared<diffuse>(color(.65, .05, .05));
	auto white = make_shared<diffuse>(color(.73, .73, .73));
	auto green = make_shared<diffuse>(color(.12, .45, .15));
	auto light = make_shared<diffuse_light>(color(15, 15, 15));
	world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
	world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
	world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
	world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
	world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
	world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));



	//rotated block in Cornell box
	shared_ptr<object> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	world.add(box1);
	shared_ptr<object> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	world.add(box2);
	world = objectlist(make_shared<bvh_node>(world));
	Scene cam;
	cam.aspect_ratio = 1.0;
	cam.img_width = 600;
	cam.samples_per_pixel = 200;
	cam.max_depth = 50;
	cam.backgroundcolor = color(0, 0, 0);
	cam.fov = 40;
	cam.lookfrom = point3(278, 278, -800);
	cam.lookat = point3(278, 278, 0);
	cam.vup = vec3(0, 1, 0);
	cam.depth_field_angle = 0;
	cam.viewport_distance = 800;
	cam.render(world,filename);
}
void perlin_spheres(const char*filename) {
	objectlist world;
	auto pertext = make_shared<noise_texture>(5);
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<diffuse>(pertext)));
	world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<diffuse>(pertext)));

	Scene cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.img_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;
	cam.fov = 20;
	cam.lookfrom = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);
	cam.viewport_distance = 13;
	cam.depth_field_angle = 0;
	cam.backgroundcolor = color(0.70, 0.80, 1.00);

	cam.render(world, filename);
}
void cornell_smoke(const char*filename) {
	objectlist world;
	auto red = make_shared<diffuse>(color(.65, .05, .05));
	auto white = make_shared<diffuse>(color(.73, .73, .73));
	auto green = make_shared<diffuse>(color(.12, .45, .15));
	auto light = make_shared<diffuse_light>(color(4,4 ,4));
	world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
	world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
	world.add(make_shared<quad>(point3(113, 554, 127), vec3(330, 0, 0), vec3(0, 0, 305), light));
	world.add(make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
	world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
	world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));
	shared_ptr<object> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	shared_ptr<object> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	world.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
	world.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));
	Scene cam;
	cam.aspect_ratio = 1.0;
	cam.img_width = 600;
	cam.samples_per_pixel = 200;
	cam.max_depth = 50;
	cam.backgroundcolor = color(0, 0, 0);
	cam.fov = 40;
	cam.lookfrom = point3(278, 278, -800);
	cam.lookat = point3(278, 278, 0);
	cam.vup = vec3(0, 1, 0);
	cam.viewport_distance = 800;
	cam.depth_field_angle = 0;
	cam.render(world,filename);
}
void final_scene(int image_width, int samples_per_pixel, int max_depth,const char*filename) {
	objectlist boxes1;
	auto ground = make_shared<diffuse>(color(0.48, 0.83, 0.53));
	int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++) {
		for (int j = 0; j < boxes_per_side; j++) {
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = random_double(1, 101);
			auto z1 = z0 + w;
			boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
		}
	}
	objectlist world;
	world.add(make_shared<bvh_node>(boxes1));
	auto light = make_shared<diffuse_light>(color(7, 7, 7));
	world.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));
	auto center1 = point3(400, 400, 200);
	auto center2 = center1 + vec3(30, 0, 0);
	auto sphere_material = make_shared<diffuse>(color(0.7, 0.3, 0.1));
	world.add(make_shared<sphere>(center1, center2, 50, sphere_material));
	world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
	world.add(make_shared<sphere>(
		point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
	));
	auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
	world.add(boundary);
	world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
	boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
	world.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));
	auto emat = make_shared<diffuse>(make_shared<Texture_color>("map.jpg"));
	world.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
	auto pertext = make_shared<noise_texture>(0.2);
	world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<diffuse>(pertext)));
	objectlist boxes2;
	auto white = make_shared<diffuse>(color(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
	}
	world.add(make_shared<translate>(
		make_shared<rotate_y>(
			make_shared<bvh_node>(boxes2), 15),
		vec3(-100, 270, 395)
	)
	);
	Scene cam;
	cam.aspect_ratio = 1.0;
	cam.img_width = image_width;
	cam.samples_per_pixel = samples_per_pixel;
	cam.max_depth = max_depth;
	cam.backgroundcolor = color(0, 0, 0);
	cam.fov = 40;
	cam.lookfrom = point3(478, 278, -600);
	cam.lookat = point3(278, 278, 0);
	cam.vup = vec3(0, 1, 0);
	cam.viewport_distance = 10;
	cam.depth_field_angle = 0;
	cam.render(world,filename);
}
int main(int argc,char* argv[])
{

	//earth(argv[1]);


	//objects = objectlist(make_shared<bvh_node>(objects));
	//非常巧妙的代码，光线传播的过程就是通过判断光线与objects中的物体碰撞的过程，将初始化好的bvh放入objects,这一行代码就引入了bvh加速
	//quads(argv[1]);
	//simple_light(argv[1]);
	//cornell_smoke(argv[1]);
	final_scene(800, 10000, 40,argv[1]);
	return 0;
}