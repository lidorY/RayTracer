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

	void r(double v) { r_ = max(v, 0.0); }
	void g(double v) { g_ = max(v, 0.0); }
	void b(double v) { b_ = max(v, 0.0); }

	double r() const { return r_; }
	double g() const { return g_; }
	double b() const { return b_; }


	uint8_t red()   const {return std::clamp(r_ * 255.0, 0.0, 255.0);}
	uint8_t blue()  const {return std::clamp(b_ * 255.0, 0.0, 255.0);}
	uint8_t green() const {return std::clamp(g_ * 255.0, 0.0, 255.0);}

private:
	double r_;
	double b_;
	double g_;

};


struct DiffuseMaterial {
	Color kd;
	Color ks;
	float specular_coef;
};

#endif /*COLOR_H*/