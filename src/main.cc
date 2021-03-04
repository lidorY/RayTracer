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


class Object {
public:

	virtual std::optional<gmtl::Vec3d> TestCollision(const Ray& r) const = 0;
	virtual Color Shade(gmtl::Vec3d point_light, gmtl::Vec3d intersec_point) = 0;

};

class Cube : public Object{

public:
	std::optional<gmtl::Vec3d> TestCollision(const Ray& r) const override {
		
	}

	Color Shade(gmtl::Vec3d point_light, gmtl::Vec3d intersec_point) override {
		
	}

private:
	gmtl::Vec3d bounds[8];
	gmtl::Vec3d max, min;

};
// Basic starting object
class Sphere : public Object{
public:
	Sphere(gmtl::Vec3d center, float radius) :
		center_(center),
		radius_(radius)
	{}


	std::optional<gmtl::Vec3d> TestCollision(const Ray& r) const override{
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

		auto t = (-b - sqrt((b * b) - 4 * a * c)) / (2 * a);

		auto dx = r.p1[0] - r.p0[0];
		auto dy = r.p1[1] - r.p0[1];
		auto dz = r.p1[2] - r.p0[2];

		return gmtl::Vec3d{ (r.p0[0]), (r.p0[1]), (r.p0[2] + t) };
		
	}

	Color Shade(gmtl::Vec3d point_light, gmtl::Vec3d intersec_point) override {
		Color res{ 255, 255, 255 };
		gmtl::Vec3d normal = (intersec_point - center_);
		normal /= radius_;

		gmtl::Vec3d light_dir = intersec_point - point_light;
		gmtl::normalize(light_dir);


		double kd = 1.1;
		double ka = 0.3;

		double fctr {0.0};
		fctr = gmtl::dot(normal, light_dir) / (gmtl::length(normal) * gmtl::length(light_dir));
	
		res.r = std::clamp(148 * ka + kd * fctr * 255, 0.0, 255.0);
		res.g = std::clamp(195 * ka + kd * fctr * 255, 0.0, 255.0);
		res.b = std::clamp(236 * ka + kd * fctr * 255, 0.0, 255.0);
		
		
		return res;
	}

private:
	gmtl::Vec3d center_;
	float radius_;

	int num;
};

void Tracer(Screen& scr) {
	//ViewFrustum vfr{scr.Width(), scr.Height(), 60.0, 1000.0};

	// TODO: refine the definition in order to avoid near/far plane cliping problems
	Sphere sphr { {270, 270, 200}, 100 };

	for (auto y = 0; y < scr.Height(); ++y) {
		for (auto x = 0; x < scr.Width(); ++x) {
			//TODO: define move semantics for Ray(avoid unecessary copy)
			Ray ray;
			ray.p0 = gmtl::Vec3d{ (float)(x), (float)(y), 0.3 };
			ray.p1 = gmtl::Vec3d{ (float)(x), (float)(y), 1000 };

			// Test for collision
			if (auto ip = sphr.TestCollision(ray)) {
				// Collision detected
				scr.StorePixel(x, y, sphr.Shade(gmtl::Vec3d{ 10, 0, -50}, ip.value()));
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