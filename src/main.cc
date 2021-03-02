#include <Windows.h>
#include <memory>
#include <iostream>

// Math includes
#include <gmtl\gmtl.h>

#include "screen.h"


struct Ray {
	gmtl::Vec3d origin;
	gmtl::Vec3d direction;
};

class PointLight {
	gmtl::Vec3d center;
};

// Basic starting object
class Sphere {
	gmtl::Vec3d center;
	float rdaius;
};

// Algorithm:
// find intersections to objects in the scene
// Draw "shadow feelers" to the existing light sources
//	- Decide which are visible by once again traversing object for collision detection
// 


class ViewFrustum {
public:
	ViewFrustum(std::size_t scr_width, 
		std::size_t scr_height, float angle) :
		width_(scr_width),
		height_(scr_height),
		aspect_ratio_(static_cast<float>(scr_width)/static_cast<float>(scr_height)),
		angle_(angle)
	{}

	Ray GetRayByPixel(int x, int y) {
		Ray res;
		
		// res.direction = ...
		// For simplicity I'm setting the "Camera" to the world origin (0,0,0)
		res.origin = { 0,0,0 };

		return res;
	}

private:
	std::size_t width_;
	std::size_t height_;
	float aspect_ratio_;
	float angle_;
};


void Tracer(Screen& scr) {
	for (auto i = 0; i < scr.Height(); ++i) {
		for (auto j = 0; j < scr.Width(); ++j) {
			Color res = { 0, 0, 0 };
			// Test for collision
			// Send shadow/light feelers
			// calculate res color based on light source
			scr.StorePixel(i, j, res);
		}
	}
}


int main() {
	HDC consoleDC = GetDC(GetConsoleWindow());
	// Define on the free store in order to enable large data storage
	std::unique_ptr screen = std::make_unique<Screen>(640, 640, consoleDC);
	
	Tracer(*screen.get());

	Color clear_color = { 148, 236, 195 };
	screen->ClearScreenColor(clear_color);
	screen->DrawScreen();

}