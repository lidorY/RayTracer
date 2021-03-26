#ifndef VIEW_FRUSTUM_H
#define VIEW_FRUSTUM_H

#include <cstddef>
#include <cmath>

#include <gmtl/gmtl.h>

#include "common.h"


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

		double phy_width = 2 * 1 * std::tan(0.5 * angle * TO_RAD);
		double phy_height = aspect_ratio_ * phy_width;

		pixel_width_ = phy_width / scr_width;
		pixel_height_ = phy_height / scr_height;
	}

	Ray&& GetRayByPixel(int x, int y) {
		// Zero terminated indexing.(staring from x/y=0)
		return {
			position{gmtl::Vec3d{0, 0, 0}},
			position{gmtl::Vec3d{
				(x + 0.5) * pixel_width_,
				(y + 0.5) * pixel_height_,
				1
			}}
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

#endif /*VIEW_FRUSTUM_H*/