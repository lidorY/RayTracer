#include <Windows.h>
#include <memory>
#include <optional>
#include <vector>
#include <chrono>

// Math includes
#include <gmtl\gmtl.h>

#include"color.h"
#include "screen.h"
#include "objects.h"
#include "view_frustum.h"




static std::vector<std::unique_ptr<Object>> objs;



Color TraceRay(Ray ray, 
	const std::vector<PointLight>& point_lights,
	const std::vector<std::unique_ptr<Object>>& colliders,
	int depth = 0) {

	// TODO: use a class to store this "static" variable
	Color bgr = Color{ 0.58, 0.76, 0.92 };

	// 1. Find the closest intersected object
	Object* current_obj = nullptr;
	double nearest = INFINITY;
	for (auto&& c : colliders) {
		if (auto ip = c->TestCollision(ray)) {
			auto t = ip.value();
			if (t < nearest) {
				nearest = t;
				current_obj = c.get();
			}
		}
	}

	if (!current_obj) {
		return bgr;
	}

	gmtl::Vec3d intersection_point = ray.origin + ray.dir * nearest;
	auto normal_at_intersec = current_obj->calcNormal(intersection_point);

	// 2. Shoot shadow feeler to decide diffuse color
	Color surface_diffuse = current_obj->Shade(point_lights, intersection_point, colliders, normal_at_intersec);

	// 3. Shoot reflection ray and trace further
	// 4. Shoot refraction ray
	
	return surface_diffuse;
}

void Tracer(Screen& scr) {

	Color bgr = Color{ 0.58, 0.76, 0.92 };
	scr.ClearScreenColor(bgr);

	ViewFrustum vfr{scr.Width(), scr.Height(), 60, 1000.0};

	Light scene_ambient_light{ bgr, 0.5f };

	objs.push_back(std::make_unique<Plane>(
		gmtl::Vec3d{ 0, -3, 0 }, gmtl::Vec3d{ 0, 1, 0 },
		DiffuseMaterial{
			Color{1.0, 1.0, 1.0} * 0.5,
			Color{1.0, 1.0, 1.0} * 0.5,
			Color{0, 0, 0} * 0.5,
			0,
			0
		},
		scene_ambient_light		
		 ));


	objs.push_back(std::make_unique<Sphere>(
		gmtl::Vec3d{ 1, 1, 10 }, 1,
		DiffuseMaterial{
			Color{0.5, 0.313, 0.64},
			Color{0.5, 0.313, 0.64},
			Color{0.5, 0.313, 0.64},
			100,
			0.0f,
			.0f,
			1.f
		},
		scene_ambient_light
		));
	
	objs.push_back(std::make_unique<Sphere>(
		gmtl::Vec3d{ 2.2, 1, 13 }, 1,
		DiffuseMaterial{
			Color{0.9, 0.4, 0.298 },
			Color{0.9, 0.4, 0.298 },
			Color{0.9, 0.4, 0.298 },
			10,
			0.0f,
			.8f,
			0.f
		},
		scene_ambient_light
		));
	

	std::vector<PointLight> lights;
	lights.push_back(PointLight{ Color{1, 1, 1}, 1.f, gmtl::Vec3d{ 0, 4, 7 }});
	lights.push_back(PointLight{ Color{1, 1, 1}, .6f, gmtl::Vec3d{ 10, 8, 3 }});

	for (auto y = 0; y < scr.Height(); ++y) {
		for (auto x = 0; x < scr.Width(); ++x) {
			scr.StorePixel(x, y, 1, 
				TraceRay(vfr.GetRayByPixel(
					x - (scr.Width() / 2),
					y - (scr.Height() / 2)
				), lights, objs));
		}
	}
}


int main() {
	auto t0 = std::chrono::high_resolution_clock::now();
	HDC consoleDC = GetDC(GetConsoleWindow());
	// Define on the free store in order to enable large data storage
	std::unique_ptr screen = std::make_unique<Screen>(640, 640, consoleDC);
	

	Tracer(*screen.get());
	screen->DrawScreen();

	auto t1 = std::chrono::high_resolution_clock::now();

	std::cout << "Time: " << std::chrono::duration<double, std::milli>(t1 - t0).count() / 1000.0 << "Seconds \n";

}