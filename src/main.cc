#include <Windows.h>
#include <memory>
#include <optional>

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
public:
	Sphere(gmtl::Vec3d center, float radius) :
		center_(center),
		radius_(radius)
	{}
	
	std::optional<gmtl::Vec3d> TestCollision(const Ray& r) const {
		// TODO: implement actual collision detection..
		//return gmtl::Vec3d{ 0,0,0 };
		return {};
	}

private:
	gmtl::Vec3d center_;
	float radius_;
};

// Algorithm:
// find intersections to objects in the scene
// Draw "shadow feelers" to the existing light sources
//	- Decide which are visible by once again traversing object for collision detection
// 


class ViewFrustum {
public:
	ViewFrustum(std::size_t scr_width, 
		std::size_t scr_height, double angle,
		double view_plane_distance) :
		scr_width_(scr_width),
		scr_height_(scr_height),
		angle_(angle),
		view_plane_distance_(view_plane_distance)
	{
		aspect_ratio_ = static_cast<double>(scr_height) / static_cast<double>(scr_width);
		double phy_width = 2 * view_plane_distance * std::tan(angle / 2);
		double phy_height = aspect_ratio_ * phy_width;

		pixel_width_ = phy_width / scr_width;
		pixel_height_ = phy_height / scr_height;
	}

	Ray GetRayByPixel(int x, int y) {
		// Zero terminated indexing.(staring from x/y=0)
		Ray res;
	
		// For simplicity I'm setting the "Camera" to the world origin (0,0,0)
		res.origin = { 0,0,0 };

		res.direction = { 
			(x * pixel_width_)  + (pixel_width_/2.f), 
			(y * pixel_height_) + (pixel_height_/2.f),
			view_plane_distance_ };

		return res;
	}

private:

	std::size_t scr_width_;
	std::size_t scr_height_;
	double aspect_ratio_;
	double angle_;

	double view_plane_distance_;
	double pixel_width_;
	double pixel_height_;
};


void Tracer(Screen& scr) {
	ViewFrustum vfr{scr.Width(), scr.Height(), 60.0, 10.0};

	// TODO: refine the definition in order to avoid near/far plane cliping problems
	Sphere sphr { {0,0,2.5}, 0.5 };

	for (auto i = 0; i < scr.Height(); ++i) {
		for (auto j = 0; j < scr.Width(); ++j) {
			Color res = { 0, 0, 0 };
			//TODO: define move semantics for Ray(avoid unecessary copy)
			auto ray = vfr.GetRayByPixel(i, j);

			// Test for collision
			if (auto p = sphr.TestCollision(ray)) {
				// Collision detected
				// Send shadow/light feelers
				// calculate res color based on light source
			} 
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