#include <Windows.h>
#include <memory>
#include <optional>
#include <vector>

// Math includes
#include <gmtl\gmtl.h>

#include "screen.h"



struct Ray {
	Ray(gmtl::Vec3d p0, gmtl::Vec3d p1) :
		origin(p0) {
		dir = (p1 - p0);
		gmtl::normalize(dir);
	}
	Ray(Ray&& r) {
		origin = r.origin;
		dir = r.dir;
	}
	gmtl::Vec3d origin;
	gmtl::Vec3d dir;
};


struct Light {
	Light(gmtl::Vec3d pos, float intensity) :
		pos(pos), intensity(intensity) {}
	gmtl::Vec3d pos;
	float intensity;
};


class Object {
public:
	Object(int spec, float ka, float kd, std::string name) :
		specularity_coef_(spec),
		ka_(ka),
		kd_(kd),
		name(name)
	{}

	virtual std::optional<double> TestCollision(const Ray& r) const = 0;


	Color Shade(const std::vector<Light>& point_lights, gmtl::Vec3d intersec_point, 
		const std::vector<std::unique_ptr<Object>>& colliders)  {
		Color res{ 0, 0, 0 };
		for (auto&& light : point_lights) {
			gmtl::Vec3d normal = calcNormal(intersec_point);
			gmtl::normalize(normal);

			gmtl::Vec3d light_dir = light.pos - intersec_point;
			gmtl::normalize(light_dir);

			double fctr{ 0.0 };
			bool occluded = false;

			// dumb ineffecient way to implement hard shdows
			for (auto&& o : colliders) {
				if (o.get() == this) { continue; }
				auto p = o->TestCollision({ intersec_point, light.pos });
				if (p.has_value()) {
					//if (p.value() < 1 && p.value() > 0) {
						// Making sure the object is between the intersection and the light source..
						// p bigger than 1 means that we might intersect but wer'e not in the way of the light ray
						occluded = true;
						//break;
					//}
				}
			}
			if (!occluded) {
				fctr = gmtl::dot(normal, light_dir) / (gmtl::length(normal) * gmtl::length(light_dir));
			}
			fctr = std::pow(fctr, specularity_coef_);

			res.r += std::clamp((kd_ * fctr * 255) * light.intensity, 0.0, 255.0);
			res.g += std::clamp((kd_ * fctr * 255) * light.intensity, 0.0, 255.0);
			res.b += std::clamp((kd_ * fctr * 255) * light.intensity, 0.0, 255.0);
		}
		// Add ambient light
		res.r = std::clamp(148 * ka_ + res.r, 0.0, 255.0);
		res.g = std::clamp(195 * ka_ + res.g, 0.0, 255.0);
		res.b = std::clamp(236 * ka_ + res.b, 0.0, 255.0);

		//return { 255, 255, 255 };
		return res;
	}

	virtual ~Object() {}

protected:
	virtual gmtl::Vec3d calcNormal(gmtl::Vec3d point) = 0;

	int specularity_coef_;
	double kd_;
	double ka_;

	// debugging purpose
	std::string name;
};

static std::vector<std::unique_ptr<Object>> objs;


class Plane : public Object {
public:
	Plane(gmtl::Vec3d origin, gmtl::Vec3d normal, int spec,
		float ka, float kd) :
		Object(spec, ka, kd, "Plane"),
		origin_(origin),
		normal_(normal)
	{
		gmtl::normalize(normal_);
	}

	virtual std::optional<double> TestCollision(const Ray& r) const {
		
		double denom = gmtl::dot(r.dir, normal_);
		if (std::abs(denom) > 0.08) {
			// Avoid zero division
			gmtl::Vec3d diff = origin_- r.origin;
			double t = gmtl::dot(diff, normal_) / denom;
			if (t > 0) {
				return t;
				/*return gmtl::Vec3d{
					r.origin[0] + t * r.dir[0],
					r.origin[1] + t * r.dir[1],
					r.origin[2] + t * r.dir[2]
				};*/
			}
		}
		return std::nullopt;
	}

private:
	virtual gmtl::Vec3d calcNormal(gmtl::Vec3d point) {
		return normal_;
	}

	gmtl::Vec3d origin_;
	gmtl::Vec3d normal_;
};

// Basic starting object
class Sphere : public Object{
public:
	Sphere(gmtl::Vec3d center, float radius, int spec,
		float ka, float kd) :
		Object(spec, ka, kd, "Sphere"),
		center_(center),
		radius_(radius)
	{}


	std::optional<double> TestCollision(const Ray& r) const override{

		//gmtl::Vec3d L = center_ - r.origin;
		//
		//double a = gmtl::dot(r.dir, r.dir);
		//double b = 2 * gmtl::dot(L, r.dir);
		//double c = gmtl::dot(L, L) - std::pow(radius_, 2);

		//auto discriminant = std::pow(b, 2) - 4 * a * c;
		//if (discriminant < 0) {
		//	return {};
		//}

		//auto t = (-b - sqrt((b * b) - 4 * a * c)) / (2 * a);
		double radius2 = std::pow(radius_, 2);
		gmtl::Vec3d L = center_ - r.origin;
		double tca = gmtl::dot(L, r.dir);
		if (tca < 0) return std::nullopt;
		double d2 = gmtl::dot(L, L) - std::pow(tca,2);
		if (d2 > radius2) return std::nullopt;
		double thc = std::sqrt(radius2 - d2);
		double t = tca - thc;
		return t;
		/*return gmtl::Vec3d{
			(r.origin[0] + t * r.dir[0]),
			(r.origin[1] + t * r.dir[1]),
			(r.origin[2] + t * r.dir[2])
		};*/
	}

private:
	gmtl::Vec3d calcNormal(gmtl::Vec3d point) override {
		return point - center_;
	}

	gmtl::Vec3d center_;
	double radius_;

};


class ViewFrustum {
	const double TO_RAD = 3.141592653589793 / 180.;
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
		
		double phy_width = 2 * 1 * std::tan( 0.5 * angle * TO_RAD);
		double phy_height = aspect_ratio_ * phy_width;

		pixel_width_ = phy_width / scr_width;
		pixel_height_ = phy_height / scr_height;
	}

	Ray&& GetRayByPixel(int x, int y) {
		// Zero terminated indexing.(staring from x/y=0)
		return {
			gmtl::Vec3d{0, 0, 0},
			gmtl::Vec3d{
				(x + 0.5) * pixel_width_,
				(y + 0.5) * pixel_height_,
				1
			}
		};
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
	ViewFrustum vfr{scr.Width(), scr.Height(), 60, 1000.0};

	objs.push_back(std::make_unique<Plane>(
		gmtl::Vec3d{0, -3, 0}, gmtl::Vec3d{0, 1, 0}, 1, .3f, 1.f
		));
	objs.push_back(std::make_unique<Sphere>(
		gmtl::Vec3d{ 4, 2, 20 }, 1, 1, 0.3f, 1.f));
	objs.push_back(std::make_unique<Sphere>(
		gmtl::Vec3d{ 0, 0, 13}, 1, 1, 0.3f, 1.f));


	std::vector<Light> lights;
	lights.push_back(Light{ gmtl::Vec3d{ 0, 8, 18 },  .8f });
	lights.push_back(Light{ gmtl::Vec3d{ 0, 8, 3 }, .6f });

	for (auto&& obj : objs) {
		for (auto y = 0; y < scr.Height(); ++y) {
			for (auto x = 0; x < scr.Width(); ++x) {
				Ray ray = vfr.GetRayByPixel(
					x - (scr.Width()/2), 
					y - (scr.Height()/2));
				// Test for collision
				if (auto ip = obj->TestCollision(ray)) {
					// Collision detected
					auto t = ip.value();
					gmtl::Vec3d intersection_point = ray.origin + ray.dir * t;
					scr.StorePixel(x, y, obj->Shade(lights , intersection_point, objs));
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