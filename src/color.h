#ifndef COLOR_H
#define COLOR_H
 
#include <algorithm>

// RGB color definition
// Values range from 0.0 - 255.0
struct Color {
public:
	Color(double r, double g, double b) :
		r_(std::clamp(r, 0.0, 255.0)),
		g_(std::clamp(g, 0.0, 255.0)),
		b_(std::clamp(b, 0.0, 255.0)) {}
	void r(double v) { r_ = std::clamp(v, 0.0, 255.0); }
	void g(double v) { g_ = std::clamp(v, 0.0, 255.0); }
	void b(double v) { b_ = std::clamp(v, 0.0, 255.0); }
	double r() const { return r_; }
	double g() const { return g_; }
	double b() const { return b_; }
private:
	double r_;
	double b_;
	double g_;
};

#endif /*COLOR_H*/