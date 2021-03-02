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


int main() {
	HDC consoleDC = GetDC(GetConsoleWindow());
	// Define on the free store in order to enable large data storage
	std::unique_ptr screen = std::make_unique<Screen>(640, 640, consoleDC);
	
	
	screen->ClearScreenColor(236,195,148);
	screen->DrawScreen();

}