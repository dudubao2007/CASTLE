#ifndef AXIS_HPP
#define AXIS_HPP
#include "array.hpp"
#include "const.hpp"
#include "global_define.hpp"
#include <cmath>
class Axis {
private:
	Float x_c, y_c;
	Float u_x, u_y;
	Float eq_u;

public:
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
	Coordinate to_point(Float x, Float y) {
		Coordinate ans;
		ans[0] = (x - x_c) / u_x;
		ans[1] = (y - y_c) / u_y;
		return ans;
	}
	Coordinate to_pixel(Float x, Float y) {
		Coordinate ans;
		ans[0] = x_c + x * u_x;
		ans[1] = y_c + y * u_y;
		return ans;
	}
	Float x_unit() { return u_x; }
	Float y_unit() { return u_y; }
	Float unit() { return eq_u; }
};
#endif