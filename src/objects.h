#ifndef OBJECTS_H
#define OBJECTS_H


#include <string>
#include <optional>


#include "common.h"
#include "color.h"

static const unsigned int MAX_RAY_DEPTH = 2;

class Object {
public:
	Object(DiffuseMaterial material , Light ambient_light, std::string name) :
		material_(material),
		ambient_light_(ambient_light),
		name(name)
		{}

	virtual std::optional<double> TestCollision(const Ray& r) const = 0;


	Color GetColorInIntersection(
		gmtl::Vec3d intersec_point,
		const std::vector<std::unique_ptr<PointLight>>& point_lights,
		const std::vector<std::unique_ptr<Object>>& colliders,
		int ray_depth = 0) {
		
		if (ray_depth > MAX_RAY_DEPTH) {
			// Recursion stop condition
			return ambient_light_.light_color * ambient_light_.intensity;
		}



		gmtl::Vec3d normal = calcNormal(intersec_point);

		// Send shadow feeler to light sources
		auto surface_light_color = Shade(point_lights, intersec_point, colliders, normal);

		gmtl::Vec3d view_direction = intersec_point;
		gmtl::normalize(view_direction);

		bool inside = false;
		if (gmtl::dot(normal, view_direction) > 0) {
			normal = -normal;
			inside = true;
		}

		// Calc relection and refraction rays
		float facing_ratio = -gmtl::dot(normal, view_direction);
		float fresnel = mix(std::pow(1 - facing_ratio, 3), 1, 0.1f);
		
		gmtl::Vec3d reflection_dir = view_direction - normal * (2 * gmtl::dot(view_direction, normal));
		gmtl::normalize(reflection_dir);

		double bias = 1e-4;
		Ray reflection = { intersec_point + normal * bias, reflection_dir };

		//Ray refraction = Ray{ gmtl::Vec3d{0,0,0}, gmtl::Vec3d{0,0,0} };
		
		// Test reflection rays intersections with colliders:
		Color reflection_value = {0,0,0}; 
		bool reflected = false;
		for (auto&& c : colliders) {
			if (c.get() == this) { continue; }
			if (auto ip = c->TestCollision(reflection)) {
				// Collision detected
				auto t = ip.value();
				gmtl::Vec3d ref_intersection_point = reflection.origin + reflection.dir * t;
				reflection_value = c->GetColorInIntersection(ref_intersection_point, point_lights, colliders, ray_depth+1);
				reflected = true;
				//break;
			}
		}

		if (!reflected) {
			// If we didn't hit any surface there's no reason to trace any further..
			return ambient_light_.light_color * ambient_light_.intensity + surface_light_color;
		}
		


		// Test refraction rays intersections with colliders:
		Color refraction_value = { 0,0,0 };
		double ior = 1.1;
		double eta = inside ? ior : 1 / ior;

		double cosi = -gmtl::dot(normal, view_direction);
		double k = 1 - std::pow(eta, 2) * (1 - std::pow(cosi, 2));

		gmtl::Vec3d ref_dir = (view_direction * eta) + (normal * (eta * cosi - std::sqrt(k)));
		gmtl::normalize(ref_dir);

		Ray refraction = Ray{ intersec_point, ref_dir };
		bool refracted = false;
		for (auto&& c : colliders) {
			if (c.get() == this) { continue; }
			if (auto ip = c->TestCollision(refraction)) {
				// Collision detected
				auto t = ip.value();
				gmtl::Vec3d ref_intersection_point = refraction.origin + refraction.dir * t;
				refraction_value = c->GetColorInIntersection(ref_intersection_point, point_lights, colliders, ray_depth + 1);
				refracted = true;
				break;
			}
		}
		if (!refracted) {
			// If we didn't hit any surface there's no reason to trace any further..
			return ambient_light_.light_color * ambient_light_.intensity + surface_light_color;
		}


		return (reflection_value * fresnel  + refraction_value * (1 - fresnel) * material_.transparency) + surface_light_color;
	}

	Color Shade(const std::vector<std::unique_ptr<PointLight>>& lights, gmtl::Vec3d intersec_point,
		const std::vector<std::unique_ptr<Object>>& colliders, gmtl::Vec3d normal) {

		// Accumulate the light color from the lights in the scene

		Color res{ 0, 0, 0 };
		for (auto&& light : lights) {
			double diff_fctr{ 0.0 };
			double spec_fctr = 0.0;
			bool occluded = false;


			double occlusion_factor = 1.0;
			 // Check for occlusions
			for (auto&& o : colliders) {
				if (o.get() == this) { continue; }

				Ray feeler{ intersec_point, light->pos };
				auto p = o->TestCollision(feeler);
				if (p.has_value()) {
					auto normal_at_intersec = o->calcNormal(feeler.origin + p.value() * feeler.dir);
					float facing_ratio = -gmtl::dot(normal_at_intersec, feeler .dir);
					float fresnel = mix(std::pow(1 - facing_ratio, 1), 0, .1f);

					//occlusion_factor = 0.0;
					occlusion_factor = (1 - fresnel) * o->material().transparency;
					break;
				}
			}

			gmtl::Vec3d light_dir = light->pos - intersec_point;
			gmtl::normalize(light_dir);
			diff_fctr = std::abs(gmtl::dot(normal, light_dir) / (gmtl::length(normal) * gmtl::length(light_dir)));

			//gmtl::Vec3d Rm = ((2 * gmtl::dot(normal, light_dir) * normal) - light_dir);
			//gmtl::normalize(intersec_point);
			//spec_fctr = std::abs(gmtl::dot(Rm, -intersec_point));
			

			// Phong reflection model
			double  r_diff = (material_.kd.r() * (light->light_color.r() * light->intensity) * diff_fctr) * occlusion_factor;
			double  g_diff = (material_.kd.g() * (light->light_color.g() * light->intensity) * diff_fctr) * occlusion_factor;
			double  b_diff = (material_.kd.b() * (light->light_color.b() * light->intensity) * diff_fctr) * occlusion_factor;

			res.r(res.r() + r_diff);
			res.b(res.b() + b_diff);
			res.g(res.g() + g_diff);
		}


		return res;
	}

	virtual ~Object() {}

	virtual gmtl::Vec3d calcNormal(gmtl::Vec3d point) = 0;

	DiffuseMaterial material() { return material_; }

	const Light& GetAmbient() { return ambient_light_; }

protected:

	DiffuseMaterial material_;
	Light ambient_light_;
	// debugging purpose
	std::string name;
};


class Plane : public Object {
public:
	Plane(gmtl::Vec3d origin, gmtl::Vec3d normal,
		DiffuseMaterial material, Light ka) :
		Object(material, ka, "Plane"),
		origin_(origin),
		normal_(normal)
	{
		gmtl::normalize(normal_);
	}

	virtual std::optional<double> TestCollision(const Ray& r) const {

		double denom = gmtl::dot(r.dir, normal_);
		if (std::abs(denom) > 1e-6) {
			// Avoid zero division
			gmtl::Vec3d diff = origin_ - r.origin;
			double t = gmtl::dot(diff, normal_) / denom;
			if (t > 0) {
				return t;
			}
		}
		return std::nullopt;
	}

	virtual gmtl::Vec3d calcNormal(gmtl::Vec3d point) {
		// TODO: Add here a throwing test?
		gmtl::normalize(normal_);
		return normal_;
	}
private:

	gmtl::Vec3d origin_;
	gmtl::Vec3d normal_;
};

// Basic starting object
class Sphere : public Object {
public:
	Sphere(gmtl::Vec3d center, float radius,
		DiffuseMaterial material, Light ka) :
		Object(material, ka, "Sphere"),
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
		if (t < 0) {
			t = tca + thc;
		}

		return t;
	}

	gmtl::Vec3d calcNormal(gmtl::Vec3d point) override {
		// TODO: not effecient, should be using a returning function 
		// so it will opt to move semantics..
		gmtl::Vec3d normal = point - center_;
		gmtl::normalize(normal);
		return normal;
	}
private:

	gmtl::Vec3d center_;
	double radius_;
};


#endif /*OBJECTS_H*/