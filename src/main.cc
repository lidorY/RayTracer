#include <Windows.h>
#include <memory>
#include <optional>

// Math includes
#include <gmtl\gmtl.h>

#include "screen.h"


struct Ray {
	gmtl::Vec3d p0;
	gmtl::Vec3d p1;
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
		gmtl::Vec3d dir = r.p1 - r.p0;
		gmtl::Vec3d L = center_ - r.p0;
		gmtl::normalize(dir);

		auto a = gmtl::dot(dir, dir);
		auto b = 2 * gmtl::dot(L, dir);
		auto c = gmtl::dot(L, L) - radius_ * radius_;

		auto discriminant = b * b - 4 * a * c;
		if (discriminant < 0) {
			return {};
		}

		auto t = (-b - std::sqrt(b * b - 4 * a * c)) / (2 * a);

		auto dx = r.p1[0] - r.p0[0];
		auto dy = r.p1[1] - r.p0[1];
		auto dz = r.p1[2] - r.p0[2];

		return gmtl::Vec3d{ (r.p0[0] + t * dx), (r.p0[1] + t * dy), (r.p0[2] + t * dz) };
		
	}

	Color Shade(gmtl::Vec3d point_light, gmtl::Vec3d intersec_point) {
		Color res{ 255, 255, 255 };
		auto normal = gmtl::Vec3d{};
		return res;
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


//class ViewFrustum {
//public:
//	ViewFrustum(std::size_t scr_width, 
//		std::size_t scr_height, double angle,
//		double view_plane_distance) :
//		scr_width_(scr_width),
//		scr_height_(scr_height),
//		angle_(angle),
//		view_plane_distance_(view_plane_distance)
//	{
//		aspect_ratio_ = static_cast<double>(scr_height) / static_cast<double>(scr_width);
//		double phy_width = 2 * view_plane_distance * std::tan(angle / 2);
//		double phy_height = aspect_ratio_ * phy_width;
//
//		pixel_width_ = phy_width / scr_width;
//		pixel_height_ = phy_height / scr_height;
//	}
//
//	Ray GetRayByPixel(int x, int y) {
//		// Zero terminated indexing.(staring from x/y=0)
//		Ray res;
//	
//		// For simplicity I'm setting the "Camera" to the world origin (0,0,0)
//		res.p1 = {
//			(x * pixel_width_) + (pixel_width_ / 2.f),
//			(y * pixel_height_) + (pixel_height_ / 2.f),
//			0.3 };
//		res.p0 = { 
//			(x * pixel_width_)  + (pixel_width_/2.f), 
//			(y * pixel_height_) + (pixel_height_/2.f),
//			view_plane_distance_ };
//		return res;
//	}
//
//private:
//
//	std::size_t scr_width_;
//	std::size_t scr_height_;
//	double aspect_ratio_;
//	double angle_;
//
//	double view_plane_distance_;
//	double pixel_width_;
//	double pixel_height_;
//};


void Tracer(Screen& scr) {
	//ViewFrustum vfr{scr.Width(), scr.Height(), 60.0, 1000.0};

	// TODO: refine the definition in order to avoid near/far plane cliping problems
	Sphere sphr { {270, 270, 200}, 100 };

	for (auto y = 0; y < scr.Height(); ++y) {
		for (auto x = 0; x < scr.Width(); ++x) {
			//TODO: define move semantics for Ray(avoid unecessary copy)
			Ray ray;// = vfr.GetRayByPixel(x, y);
			//ray.p0 = gmtl::Vec3d{ 0, 0, 0 };
			ray.p0 = gmtl::Vec3d{ (float)(x), (float)(y), 0.3 };
			ray.p1 = gmtl::Vec3d{ (float)(x), (float)(y), 1000 };

			// Test for collision
			if (auto ip = sphr.TestCollision(ray)) {
				// Collision detected
				scr.StorePixel(x, y, sphr.Shade(gmtl::Vec3d{100, 100, 100}, ip.value()));
				// Send shadow/light feelers
				// calculate res color based on light source
			} 
		}
	}
}


int main() {
	HDC consoleDC = GetDC(GetConsoleWindow());
	// Define on the free store in order to enable large data storage
	std::unique_ptr screen = std::make_unique<Screen>(640, 640, consoleDC);
	Color clear_color = { 148, 236, 195 };
    screen->ClearScreenColor(clear_color);

	Tracer(*screen.get());


	screen->DrawScreen();

}