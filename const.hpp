#ifndef CONST_HPP
#define CONST_HPP
#include "global_define.hpp"
#include <cmath>
namespace Const {
const Float pi = std::acos((Float)-1.0);
const Float tau = 2.0 * pi;
const Float half_pi = 0.5 * pi;
const Float e = std::exp((Float)1.0);
const Float NaN = nan("");
const Float inf = 1.0 / 0.0;
} // namespace Const
namespace Func {
Float linear(Float t) { return t; }
Float sigmoid(Float t) { return 1.0 / (1.0 + std::exp(-t)); }
Float clip(Float t, Float a, Float b) {
	if (t < a)
		return a;
	if (t > b)
		return b;
	return t;
}
Float smooth(Float t) {
	const Float inflection = 10.0;
	Float error = sigmoid(-inflection * 0.5);
	return clip((sigmoid(inflection * (t - 0.5)) - error) / (1.0 - 2.0 * error),
		0.0, 1.0);
}
Float rush_into(Float t) { return 2.0 * smooth(t * 0.5); }
Float rush_from(Float t) { return 2.0 * smooth(t * 0.5 + 0.5) - 1.0; }
Float slow_into(Float t) { return std::sqrt(1.0 - (1.0 - t) * (1.0 - t)); }
Float double_smooth(Float t) {
	if (t < 0.5)
		return 0.5 * smooth(2.0 * t);
	else
		return 0.5 * (1 + smooth(2.0 * t - 1.0));
}
} // namespace Func
#endif