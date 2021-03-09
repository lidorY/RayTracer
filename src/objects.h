#ifndef OBJECTS_H
#define OBJECTS_H


#include <string>
#include <optional>


#include "common.h"
#include "color.h"

class Object {
public:
	Object(DiffuseMaterial material , Color ka, float ambient_intensity, std::string name) :
		material_(material),
		ka_(ka),
		name(name),
		ambient_intensity_(ambient_intensity)
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

			double diff_fctr{ 0.0 };
			double spec_fctr = 0.0;
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
				diff_fctr = gmtl::dot(normal, light_dir) / (gmtl::length(normal) * gmtl::length(light_dir));

				gmtl::Vec3d Rm = ((2 * gmtl::dot(normal, light_dir) * normal) - light_dir);
				gmtl::normalize(intersec_point);
				spec_fctr = gmtl::dot(Rm, intersec_point);
			}

			// Phong reflection model
			auto red =
				// diffuse
				(material_.kd.r() * diff_fctr * light.intensity)
				// Specualr
				 + (material_.ks.r() * std::pow(spec_fctr, material_.specular_coef) * light.intensity);

			auto blue =
				// diffuse
				(material_.kd.b() * diff_fctr * light.intensity)
				// Specualr
				 + (material_.ks.b() * std::pow(spec_fctr, material_.specular_coef) * light.intensity);

			auto green =
				// diffuse
				(material_.kd.g() * diff_fctr * light.intensity)
				// Specualr
				 + (material_.ks.g() * std::pow(spec_fctr, material_.specular_coef) * light.intensity);

			res.r(res.r() + red);
			res.b(res.b() + red);
			res.g(res.g() + red);
		}

		// Add ambient light
		res.r(ka_.r() * ambient_intensity_ + res.r());
		res.g(ka_.b() * ambient_intensity_ + res.g());
		res.b(ka_.g() * ambient_intensity_ + res.b());

		//return { 255, 255, 255 };
		return res;
	}

	virtual ~Object() {}

protected:
	virtual gmtl::Vec3d calcNormal(gmtl::Vec3d point) = 0;

	DiffuseMaterial material_;
	Color ka_;
	float ambient_intensity_;

	// debugging purpose
	std::string name;
};


class Plane : public Object {
public:
	Plane(gmtl::Vec3d origin, gmtl::Vec3d normal,
		DiffuseMaterial material, Color ka, float ambient_intensity
		) :
		Object(material, ka, ambient_intensity, "Plane"),
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
	Sphere(gmtl::Vec3d center, float radius,
		DiffuseMaterial material, Color ka, float ambient_intensity
	) :
		Object(material, ka, ambient_intensity, "Sphere"),
		center_(center),
		radius_(radius)
	{}


	std::optional<double> TestCollision(const Ray& r) const override {
		double radius2 = std::pow(radius_, 2);
		gmtl::Vec3d L = center_ - r.origin;
		double tca = gmtl::dot(L, r.dir);
		if (tca < 0) return std::nullopt;
		double d2 = gmtl::dot(L, L) - std::pow(tca, 2);
		if (d2 > radius2) return std::nullopt;
		double thc = std::sqrt(radius2 - d2);
		double t = tca - thc;
		return t;
	}

private:
	gmtl::Vec3d calcNormal(gmtl::Vec3d point) override {
		return point - center_;
	}

	gmtl::Vec3d center_;
	double radius_;
};


#endif /*OBJECTS_H*/