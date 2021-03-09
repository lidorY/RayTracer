#ifndef OBJECTS_H
#define OBJECTS_H


#include <string>
#include <optional>


#include "common.h"
#include "color.h"

class Object {
public:
	Object(int spec, float ka, float kd, Color diffuse_color, std::string name) :
		specularity_coef_(spec),
		ka_(ka),
		kd_(kd),
		name(name),
		diffuse_color_(diffuse_color)
	{}

	virtual std::optional<double> TestCollision(const Ray& r) const = 0;


	Color Shade(const std::vector<Light>& point_lights, gmtl::Vec3d intersec_point,
		const std::vector<std::unique_ptr<Object>>& colliders) {
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

			auto light_color_addition =
				kd_ * fctr * diffuse_color_.r();
				res.r(res.r() + fctr * (kd_ * diffuse_color_.r()) * light.intensity);
			res.b(res.b() + fctr * (kd_ * diffuse_color_.b()) * light.intensity);
			res.g(res.g() + fctr * (kd_ * diffuse_color_.g()) * light.intensity);
		}

		// Add ambient light
		res.r(148 * ka_ + res.r());
		res.g(195 * ka_ + res.g());
		res.b(236 * ka_ + res.b());

		//return { 255, 255, 255 };
		return res;
	}

	virtual ~Object() {}

protected:
	virtual gmtl::Vec3d calcNormal(gmtl::Vec3d point) = 0;

	int specularity_coef_;
	double kd_;
	double ka_;
	Color diffuse_color_;


	// debugging purpose
	std::string name;
};


class Plane : public Object {
public:
	Plane(gmtl::Vec3d origin, gmtl::Vec3d normal, int spec,
		float ka, float kd, Color diffuse_color) :
		Object(spec, ka, kd, diffuse_color, "Plane"),
		origin_(origin),
		normal_(normal)
	{
		gmtl::normalize(normal_);
	}

	virtual std::optional<double> TestCollision(const Ray& r) const {

		double denom = gmtl::dot(r.dir, normal_);
		if (std::abs(denom) > 0.08) {
			// Avoid zero division
			gmtl::Vec3d diff = origin_ - r.origin;
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
class Sphere : public Object {
public:
	Sphere(gmtl::Vec3d center, float radius, int spec,
		float ka, float kd, Color diffuse_color) :
		Object(spec, ka, kd, diffuse_color, "Sphere"),
		center_(center),
		radius_(radius)
	{}


	std::optional<double> TestCollision(const Ray& r) const override {

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
		double d2 = gmtl::dot(L, L) - std::pow(tca, 2);
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


#endif /*OBJECTS_H*/