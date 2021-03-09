#ifndef COMMON_H
#define COMMON_H

#include <gmtl/gmtl.h>

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

#endif /*COMMON_H*/