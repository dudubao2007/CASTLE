#ifndef COLOR_HPP
#define COLOR_HPP
#include "array.hpp"
#include "global_define.hpp"
#include <array>
#include <cassert>
struct Color {
	Byte r, g, b;
	Color() {}
	Color(Byte r, Byte g, Byte b)
		: r(r)
		, g(g)
		, b(b) {}

	bool operator==(Color c) { return r == c.r && g == c.g && b == c.b; }
};
class ColorExt {
	Float _r, _g, _b, _a;
	Float fit(Float x) { return (x < 0) ? 0.0 : ((x > 1) ? 1.0 : x); }
	Byte F2B(Float x) { return Byte(fit(x) * 256); }
	Float B2F(Byte x) { return x / static_cast<Float>(255.0); }

public:
	ColorExt() {}
	ColorExt(Float r, Float g, Float b, Float a = 0)
		: _r(r)
		, _g(g)
		, _b(b)
		, _a(a) {
		fit(_a), fit(_b), fit(_g), fit(_r);
	}
	ColorExt(std::array<Float, 4> c)
		: ColorExt(c[0], c[1], c[2], c[3]) {}
	ColorExt(std::array<Float, 3> c)
		: ColorExt(c[0], c[1], c[2]) {}
	ColorExt(Color c)
		: ColorExt(B2F(c.r), B2F(c.g), B2F(c.b)) {}
	ColorExt(std::array<Byte, 3> c)
		: ColorExt(B2F(c[0]), B2F(c[1]), B2F(c[2])) {}
	operator Color() { return Color(F2B(_r), F2B(_g), F2B(_b)); }
	operator std::array<Float, 4>() {
		return std::array<Float, 4>({_r, _g, _b, _a});
	}
	operator std::array<Float, 3>() {
		return std::array<Float, 3>({_r, _g, _b});
	}
	operator std::array<Byte, 3>() {
		return std::array<Byte, 3>({F2B(_r), F2B(_g), F2B(_b)});
	}
	Float r() { return _r; }
	Float g() { return _g; }
	Float b() { return _b; }
	Float a() { return _a; }
	void compose(ColorExt c) {
		if (c.a() == 0) {
			_r = c.r();
			_g = c.g();
			_b = c.b();
			_a = 0;
			return;
		}
		if (c.a() == 1) {
			return;
		}
		if (_a == 0) {
			_r = c.r() + (_r - c.r()) * c.a();
			_g = c.g() + (_g - c.g()) * c.a();
			_b = c.b() + (_b - c.b()) * c.a();
			_a = 0.0;
			return;
		}
		if (_a == 1) {
			_r = c.r();
			_g = c.g();
			_b = c.b();
			_a = c.a();
			return;
		}
		_r = (c.r() * (1.0 - c.a()) + c.a() * _r * (1.0 - _a))
			/ (1.0 - _a * c.a());
		_g = (c.g() * (1.0 - c.a()) + c.a() * _g * (1.0 - _a))
			/ (1.0 - _a * c.a());
		_b = (c.b() * (1.0 - c.a()) + c.a() * _b * (1.0 - _a))
			/ (1.0 - _a * c.a());
		_a = _a * c.a();
	}
};
#endif