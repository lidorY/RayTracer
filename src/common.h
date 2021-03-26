#ifndef COMMON_H
#define COMMON_H

#include <gmtl/gmtl.h>

#include "color.h"



struct Ray {
	typedef gmtl::Vec3d direction;
	typedef gmtl::Vec3d position;

	//Ray(position p0, direction dir) :
	//origin(p0),
	//dir(dir)
	//{}

	Ray(position p0, position p1) :
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
	position origin;
	direction dir;
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