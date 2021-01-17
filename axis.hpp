#ifndef AXIS_HPP
#define AXIS_HPP
#include "const.hpp"
#include "coordinate.hpp"
#include "global_define.hpp"
#include <cmath>
class Axis {
private:
	Float x_c, y_c;
	Float u_x, u_y;
	Float eq_u;

public:
	Axis(Float x_c, Float y_c, Float u)
		: x_c(x_c)
		, y_c(y_c)
		, u_x(u)
		, u_y(u)
		, eq_u(u) {}
	Axis(Float x_c, Float y_c, Float u_x, Float u_y)
		: x_c(x_c)
		, y_c(y_c)
		, u_x(u_x)
		, u_y(u_y) {
		Float ratio = u_x / u_y;
		if (0.99 < ratio && ratio < 1.01)
			eq_u = sqrt(u_x * u_y);
		else
			eq_u = Const::NaN;
	}
	Coordinate<Float> to_point(Float x, Float y) {
		return Coordinate<Float>((x - x_c) / u_x, (y - y_c) / u_y);
	}
	Float to_x(Float x) { return (x - x_c) / u_x; }
	Float to_y(Float y) { return (y - y_c) / u_y; }
	Coordinate<Float> to_pixel(Float x, Float y) {
		return Coordinate<Float>(x_c + x * u_x, y_c + y * u_y);
	}
	Float to_px(Float x) { return x_c + x * u_x; }
	Float to_py(Float y) { return y_c + y * u_y; }
	Float x_unit() { return u_x; }
	Float y_unit() { return u_y; }
	Float unit() { return eq_u; }
};
#endif
