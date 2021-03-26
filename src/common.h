#ifndef COMMON_H
#define COMMON_H

#include <gmtl/gmtl.h>

#include "color.h"

struct position {
	gmtl::Vec3d value;
};

struct direction {
	gmtl::Vec3d value;
};

struct Ray {

	Ray(const position& p0, const direction& dir) :
		origin(p0.value) {
		this->dir = (dir.value);
		gmtl::normalize(this->dir);
	}

	Ray(const position& p0, const position& p1) :
		origin(p0.value) {
		dir = (p1.value - p0.value);
		gmtl::normalize(dir);
	}

	Ray(const Ray& rhs) {
		origin = rhs.origin;
		dir = rhs.dir;
	}

	Ray(Ray&& r) noexcept {
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