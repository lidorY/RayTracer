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