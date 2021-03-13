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



void Tracer(Screen& scr) {

	Color bgr = Color{ 0.58, 0.76, 0.92 };

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
		gmtl::Vec3d{ 0, 1, 10 }, 1,
		DiffuseMaterial{
			Color{0.5, 0.313, 0.64},
			Color{0.5, 0.313, 0.64},
			Color{0.5, 0.313, 0.64},
			100,
			0.5f
		},
		scene_ambient_light
		));
	
	objs.push_back(std::make_unique<Sphere>(
		gmtl::Vec3d{ 4, 1.5, 15 }, 1,
		DiffuseMaterial{
			Color{0.9, 0.4, 0.298 },
			Color{0.9, 0.4, 0.298 },
			Color{0.9, 0.4, 0.298 },
			10,
			0.0f
		},
		scene_ambient_light
		));
	

	std::vector<PointLight> lights;
	lights.push_back(PointLight{ Color{1, 1, 1}, 1.f, gmtl::Vec3d{ 0, 4, 7 }});
	//lights.push_back(PointLight{ Color{1, 1, 1}, .6f, gmtl::Vec3d{ 10, 8, 3 }});

	for (auto y = 0; y < scr.Height(); ++y) {
		for (auto x = 0; x < scr.Width(); ++x) {
			for (auto&& obj : objs) {

				Ray Illum_ray = vfr.GetRayByPixel(
					x - (scr.Width()/2), 
					y - (scr.Height()/2)
				);
				
				// Test for collision
				if (auto ip = obj->TestCollision(Illum_ray)) {
					// Collision detected
					auto t = ip.value();
					gmtl::Vec3d intersection_point = Illum_ray.origin + Illum_ray.dir * t;

					uint8_t z_val = std::clamp((1 - (intersection_point[2] / 1000.0)) * 255, 0.0, 255.0);
					//scr.StorePixel(x, y, z_val, obj->Shade(lights, intersection_point, objs));
					scr.StorePixel(x, y, z_val, obj->GetColorInIntersection(intersection_point, lights, objs));
				}
			
			}
		}
	}
}


int main() {
	auto t0 = std::chrono::high_resolution_clock::now();
	HDC consoleDC = GetDC(GetConsoleWindow());
	// Define on the free store in order to enable large data storage
	std::unique_ptr screen = std::make_unique<Screen>(640, 640, consoleDC);
	
	Color clear_color = { 148, 195, 236 };
    screen->ClearScreenColor(clear_color);
	Tracer(*screen.get());
	screen->DrawScreen();

	auto t1 = std::chrono::high_resolution_clock::now();

	std::cout << "Time: " << std::chrono::duration<double, std::milli>(t1 - t0).count() / 1000.0 << "Seconds \n";

}