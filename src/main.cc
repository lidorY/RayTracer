#include <Windows.h>
#include <memory>
#include <optional>
#include <vector>
#include <chrono>

// Math includes
#include <gmtl\gmtl.h>

#include"color.h"
#include "screen.h"
#include "objects.h"
#include "view_frustum.h"






static const unsigned int MAX_DEPTH = 0;


Color TraceRay(
	const Color& bgr,
	Ray ray, 
	const std::vector< std::unique_ptr<PointLight>>& point_lights,
	const std::vector<std::unique_ptr<Object>>& colliders,
	int depth = 0) {
	// This methos is responsible of Handling Illumination 
	// Curretly using Phong Illumination model for pure diffuse objects
	// With conjuction of the Whitted liiumination algorithm for 
	// reflective and transparent objects

	if (depth > MAX_DEPTH) {
		// Recursion stop condition
		return bgr;
	}

	// 1. Find the closest intersected object
	Object* current_obj = nullptr;
	double nearest = INFINITY;
	for (auto&& c : colliders) {
		if (auto ip = c->TestCollision(ray)) {
			auto t = ip.value();
			if (t < nearest) {
				nearest = t;
				current_obj = c.get();
			}
		}
	}

	if (!current_obj) {
		return bgr;
	}

	gmtl::Vec3d intersection_point = ray.origin + ray.dir * nearest;
	auto normal_at_intersec = current_obj->calcNormal(intersection_point);

	// 2. Shoot shadow feeler to decide diffuse color
	Color surface_diffuse = current_obj->Shade(point_lights, intersection_point, colliders, normal_at_intersec);

	// 3. Shoot reflection ray and trace further
	gmtl::Vec3d view_direction = intersection_point - ray.origin;
	gmtl::normalize(view_direction);

	bool inside = false;
	if (gmtl::dot(normal_at_intersec, view_direction) > 0) {
		normal_at_intersec = -normal_at_intersec;
		inside = true;
	}

	// Calc relection and refraction rays
	float facing_ratio = -gmtl::dot(normal_at_intersec, view_direction);
	float fresnel = mix(std::pow(1 - facing_ratio, 3), 1, 0.1f);
	gmtl::Vec3d reflection_dir = view_direction - normal_at_intersec * (2 * gmtl::dot(view_direction, normal_at_intersec));
	gmtl::normalize(reflection_dir);

	double bias = 1e-4;
	Ray reflection = { intersection_point + normal_at_intersec * bias, reflection_dir };
	auto reflection_value = TraceRay(bgr, reflection, point_lights, colliders, depth + 1) * current_obj->material().reflectivity;

	// 4. Shoot refraction ray
	double ior = 1.1;
	double eta = inside ? ior : 1 / ior;

	double cosi = -gmtl::dot(normal_at_intersec, view_direction);
	double k = 1 - std::pow(eta, 2) * (1 - std::pow(cosi, 2));

	gmtl::Vec3d ref_dir = (view_direction * eta) + (normal_at_intersec * (eta * cosi - std::sqrt(k)));
	gmtl::normalize(ref_dir);

	Ray refraction = Ray{ intersection_point, ref_dir };
	auto refraction_value = TraceRay(bgr, refraction, point_lights, colliders, depth + 1) * current_obj->material().transparency;


	return (reflection_value * fresnel) +  surface_diffuse;
	//return (reflection_value * fresnel + refraction_value * (1 - fresnel)) +  surface_diffuse;
	//return surface_diffuse;
}

void Tracer(
	const Color& bgr,
	Screen& scr,
	const std::vector<std::unique_ptr<Object>>& objs,
	const std::vector<std::unique_ptr<PointLight>>& lights
	) {
	// Main ray tracing function
	// responsible of shooting the basic primary rays into the scene
	// for every relevant pixel as defined by the view frustum and the screen

	// Defin the view frustum
	ViewFrustum vfr{scr.Width(), scr.Height(), 60, 1000.0};

	// Trace every relevnat pixel on the screen
	// i.e. generate and send primary rays
	for (auto y = 0; y < scr.Height(); ++y) {
		for (auto x = 0; x < scr.Width(); ++x) {
			scr.StorePixel(x, y, 1, 
				TraceRay(
					bgr,
					vfr.GetRayByPixel(
					x - (scr.Width() / 2),
					y - (scr.Height() / 2)
				), lights, objs));
		}
	}
}


void SetupScene(
	std::vector<std::unique_ptr<Object>>& objs,
	std::vector<std::unique_ptr<PointLight>>& lights,
	Color& bgr
) {
	// Setting up the scene incudes:
	//		- Define Ambient and point lights in the scene,
	//		- Place Objects in the scene

	Light scene_ambient_light{ bgr, 0.5f };

	objs.push_back(std::make_unique<Plane>(
		gmtl::Vec3d{ 0, -3, 0 }, gmtl::Vec3d{ 0, 1, 0 },
		DiffuseMaterial{
			Color{1.0, 1.0, 1.0} *0.5,
			Color{1.0, 1.0, 1.0} *0.5,
			Color{0, 0, 0} *0.5,
			0,
			0,
			0.5f,0
		},
		scene_ambient_light
		));


	objs.push_back(std::make_unique<Sphere>(
		gmtl::Vec3d{ 1, 1, 10 }, 1,
		DiffuseMaterial{
			Color{0.5, 0.313, 0.64},
			Color{0.5, 0.313, 0.64},
			Color{0.5, 0.313, 0.64},
			100,
			0.0f,
			1.0f,
			1.f
		},
		scene_ambient_light
		));


	objs.push_back(std::make_unique<Sphere>(
		gmtl::Vec3d{ 4, 1, 13 }, 1,
		DiffuseMaterial{
			Color{0.9, 0.4, 0.298 },
			Color{0.9, 0.4, 0.298 },
			Color{0.9, 0.4, 0.298 },
			10,
			0.0f,
			2.f,
			0.f
		},
		scene_ambient_light
		));

	lights.push_back(
		std::make_unique<PointLight>(
			Color{ 1, 1, 1 },
			1.f,
			gmtl::Vec3d{ 0, 4, 7 }
	));
	//lights.push_back(PointLight{ Color{1, 1, 1}, .3f, gmtl::Vec3d{ 10, 8, 3 }});
}


int main() {
	auto t0 = std::chrono::high_resolution_clock::now();
	HDC consoleDC = GetDC(GetConsoleWindow());
	// Define on the free store in order to enable large data storage
	std::unique_ptr screen = std::make_unique<Screen>(640, 640, consoleDC);
	
	// Set up scene	
	std::vector<std::unique_ptr<Object>> objs;
	std::vector<std::unique_ptr<PointLight>> lights;

	Color bgr = Color{ 0.58, 0.76, 0.92 };
	screen->ClearScreenColor(bgr);
	SetupScene(objs, lights, bgr);

	// Apply tracing algorithm
	Tracer(
		bgr,
		*screen.get(),
		objs,
		lights);


	// Draw result
	screen->DrawScreen();

	auto t1 = std::chrono::high_resolution_clock::now();
	// Calculate the simulation time
	std::cout << "Time: " << std::chrono::duration<double, std::milli>(t1 - t0).count() / 1000.0 << "Seconds \n";
}