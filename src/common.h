#ifndef COMMON_H
#define COMMON_H

#include <gmtl/gmtl.h>

#include "color.h"

struct Ray {
	Ray(gmtl::Vec3d p0, gmtl::Vec3d p1) :
		origin(p0) {
		dir = (p1 - p0);
		gmtl::normalize(dir);
	}

	Ray(const Ray& rhs) {
		origin = rhs.origin;
		dir = rhs.dir;
	}

	Ray(Ray&& r) {
		origin = r.origin;
		dir = r.dir;
	}
	gmtl::Vec3d origin;
	gmtl::Vec3d dir;
};




struct Light {
	Light(Color c, float intensity) :
		light_color(c), intensity(intensity) {}
	Color light_color;
	float intensity;
};

struct PointLight : public Light {
	PointLight(Color c, float intensity, gmtl::Vec3d pos) :
		Light(c, intensity),
		pos(pos) {}

	gmtl::Vec3d pos;
};



#endif /*COMMON_H*/