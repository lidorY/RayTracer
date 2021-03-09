#include <Windows.h>
#include <memory>
#include <optional>
#include <vector>

// Math includes
#include <gmtl\gmtl.h>

#include"color.h"
#include "screen.h"
#include "objects.h"
#include "view_frustum.h"




static std::vector<std::unique_ptr<Object>> objs;



void Tracer(Screen& scr) {
	ViewFrustum vfr{scr.Width(), scr.Height(), 60, 1000.0};

	objs.push_back(std::make_unique<Plane>(
		gmtl::Vec3d{ 0, -1, 0 }, gmtl::Vec3d{ 0, 1, 0 }, 1, .3f, 1.f, Color{255, 255, 255}));
	objs.push_back(std::make_unique<Sphere>(
		gmtl::Vec3d{ 0, 1, 10}, 1, 1, 0.3f, 1.f, Color{130, 80, 165}
		));
	objs.push_back(std::make_unique<Sphere>(
		gmtl::Vec3d{ 4, 2, 15 }, 1, 1, 0.3f, 1.f, Color{ 230, 104, 76 }));
	

	std::vector<Light> lights;
	lights.push_back(Light{ gmtl::Vec3d{ 0, 8, 18 },  .8f });
	lights.push_back(Light{ gmtl::Vec3d{ 0, 8, 3 }, .6f });

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
					scr.StorePixel(x, y, z_val, obj->Shade(lights , intersection_point, objs));
				}
			}
		}
	}
}


int main() {
	HDC consoleDC = GetDC(GetConsoleWindow());
	// Define on the free store in order to enable large data storage
	std::unique_ptr screen = std::make_unique<Screen>(640, 640, consoleDC);
	Color clear_color = { 148, 195, 236 };
    screen->ClearScreenColor(clear_color);

	Tracer(*screen.get());


	screen->DrawScreen();

}