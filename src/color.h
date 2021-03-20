#ifndef COLOR_H
#define COLOR_H
 
#include <algorithm>

// RGB color definition
struct Color {
public:
	Color(double r, double g, double b) :
		r_(r),
		g_(g),
		b_(b) {}
	
	// Color can't be less than zero..
	// I do support more than one values in order to enable light "accumulation"
	void r(double v) { r_ = max(v, 0.0); }
	void g(double v) { g_ = max(v, 0.0); }
	void b(double v) { b_ = max(v, 0.0); }

	double r() const { return r_; }
	double g() const { return g_; }
	double b() const { return b_; }

	// Getting the integer corresponding values of the colors
	// Trim the color to correctly represent color values 
	// meaning between 0.0 (no illumination) - 1.0 (Fully lit)
	uint8_t red()   const {return std::clamp(r_, 0.0, 1.0) * 255;}
	uint8_t blue()  const {return std::clamp(b_, 0.0, 1.0) * 255;}
	uint8_t green() const {return std::clamp(g_, 0.0, 1.0) * 255;}

	Color operator*(double rhs) const {
		return {r_ * rhs, g_ * rhs, b_ * rhs};
	}

	Color operator*(const Color& rhs) {
		return Color{
		r_ * rhs.r_,
		g_ * rhs.g_,
		b_ * rhs.b_,
		};
	}

	Color& operator+=(Color rhs) {
		this->r_ += rhs.r_;
		this->g_ += rhs.g_;
		this->b_ += rhs.b_;

		return *this;
	}

	Color operator+(Color& rhs){
		return {
			r_ + rhs.r_,
			g_ + rhs.g_,
			b_ + rhs.b_,
		};
	}

private:
	double r_;
	double b_;
	double g_;

};

struct DiffuseMaterial {
	Color kd;
	Color ka;
	Color ks;
	float specular_coef;
	float spec_intensity;

	float reflectivity = 1.f;
	float transparency = 0.f;
};


float mix(const float a, const float b, const float amount) {
	return (b * amount) + a * (1 - amount);
}

#endif /*COLOR_H*/