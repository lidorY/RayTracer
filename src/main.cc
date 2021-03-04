#include <Windows.h>
#include <memory>
#include <optional>
#include <vector>

// Math includes
#include <gmtl\gmtl.h>

#include "screen.h"


struct Ray {
	gmtl::Vec3d p0;
	gmtl::Vec3d p1;
};


struct Light {
	Light(gmtl::Vec3d pos, float intensity) :
		pos(pos), intensity(intensity) {}
	gmtl::Vec3d pos;
	float intensity;
};


class Object {
public:

	virtual std::optional<gmtl::Vec3d> TestCollision(const Ray& r) const = 0;
	virtual Color Shade(const std::vector<Light>& point_lights, gmtl::Vec3d intersec_point) = 0;

};

class Cube : public Object{

public:
	std::optional<gmtl::Vec3d> TestCollision(const Ray& r) const override {
		
	}

	Color Shade(const std::vector<Light>& point_lights, gmtl::Vec3d intersec_point) override {
		
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

	Color Shade(const std::vector<Light>& point_lights, gmtl::Vec3d intersec_point) override {
		double kd = 0.7;
		double ka = 0.5;
		Color res{ 0, 0, 0 };
		for (auto&& light : point_lights) {
			gmtl::Vec3d normal = (intersec_point - center_);
			normal /= radius_;

			gmtl::Vec3d light_dir = light.pos - intersec_point;
			gmtl::normalize(light_dir);

			double fctr{ 0.0 };
			fctr = gmtl::dot(normal, light_dir) / (gmtl::length(normal) * gmtl::length(light_dir));

			res.r += std::clamp((kd * fctr * 255) * light.intensity, 0.0, 255.0);
			res.g += std::clamp((kd * fctr * 255) * light.intensity, 0.0, 255.0);
			res.b += std::clamp((kd * fctr * 255) * light.intensity, 0.0, 255.0);
		}
		// Add ambient light
		res.r = std::clamp(148 * ka + res.r, 0.0, 255.0);
		res.g = std::clamp(195 * ka + res.g, 0.0, 255.0);
		res.b = std::clamp(236 * ka + res.b, 0.0, 255.0);

		return res;
	}

private:
	gmtl::Vec3d center_;
	float radius_;

	int num;
};


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
		double phy_width = -2 * view_plane_distance * std::tan((angle / 2));
		double phy_height = aspect_ratio_ * phy_width;

		pixel_width_ = phy_width / scr_width;
		pixel_height_ = phy_height / scr_height;
	}

	Ray GetRayByPixel(int x, int y) {
		// Zero terminated indexing.(staring from x/y=0)
		Ray res;
	
		// For simplicity I'm setting the "Camera" to the world origin (0,0,0)
		res.p1 = {
			(x * pixel_width_) + (pixel_width_ / 2.f),
			(y * pixel_height_) + (pixel_height_ / 2.f),
			0.3 };
		res.p0 = { 
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
	ViewFrustum vfr{scr.Width(), scr.Height(), 60.0, 1000.0};

	std::vector<std::unique_ptr<Object>> objs;
	
	objs.push_back(std::make_unique<Sphere>(gmtl::Vec3d{ 5000, 7000, 10000 }, 2000));
	//objs.push_back(std::make_unique<Sphere>(gmtl::Vec3d{ 5000, 5000, 1000 }, 2000));
	//objs.push_back(std::make_unique<Sphere>(gmtl::Vec3d{ 370, 280, 100 }, 100));

	std::vector<Light> lights;
	lights.push_back(Light{ gmtl::Vec3d{ 0, 12000, 0 },  1.f });
	lights.push_back(Light{ gmtl::Vec3d{ 640, 0, 0 }, .2f });

	for (auto&& obj : objs) {
		for (auto y = 0; y < scr.Height(); ++y) {
			for (auto x = 0; x < scr.Width(); ++x) {
				//TODO: define move semantics for Ray(avoid unecessary copy)
				Ray ray;
				//ray.p0 = gmtl::Vec3d{ (float)(x), (float)(y), 0.3 };
				//ray.p1 = gmtl::Vec3d{ (float)(x), (float)(y), 1000 };
				ray = vfr.GetRayByPixel(x,y);
				// Test for collision
				if (auto ip = obj->TestCollision(ray)) {
					// Collision detected
					scr.StorePixel(x, y, obj->Shade(lights , ip.value()));
				}
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