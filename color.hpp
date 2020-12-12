#ifndef COLOR_HPP
#define COLOR_HPP

#include "const.hpp"
#include "global_define.hpp"
#include <array>
#include <cassert>

struct Color {
	// red, green, blue
	Byte r, g, b;

	Color()
		: Color(0, 0, 0) {}
	Color(Byte r, Byte g, Byte b)
		: r(r)
		, g(g)
		, b(b) {}
	Color(const std::array<Byte, 3> &c)
		: Color(c[0], c[1], c[2]) {}

	operator std::array<Byte, 3>() const {
		return std::array<Byte, 3>({r, g, b});
	}

	bool operator==(const Color &c) const {
		return r == c.r && g == c.g && b == c.b;
	}
};

// 更高精度的颜色类
class ColorExt {
	// red, green, blue, alpha
	// _a == 0: 不透明, _a == 1: 完全透明
	Float _r, _g, _b, _a;

	static Float fit(Float x) { return x < 0 ? 0.0 : (x > 1 ? 1.0 : x); }

	static Byte F2B(Float x) {
		if (x <= 0.0)
			return 0;
		if (x >= 1.0)
			return 255;
		return Byte(x * 256);
	}

	// 因为今后可能会 typedef double Float; 所以不能简单写成 255.0f
	static Float B2F(Byte x) { return x / static_cast<Float>(255.0); }

public:
	ColorExt() { _r = _g = _b = _a = 0; }

	ColorExt(Float r, Float g, Float b, Float a = 0)
		: _r(fit(r))
		, _g(fit(g))
		, _b(fit(b))
		, _a(fit(a)) {}

	ColorExt(const std::array<Float, 4> &c)
		: ColorExt(c[0], c[1], c[2], c[3]) {}

	ColorExt(const std::array<Float, 3> &c)
		: ColorExt(c[0], c[1], c[2]) {}

	ColorExt(const Color &c)
		: ColorExt(B2F(c.r), B2F(c.g), B2F(c.b)) {}

	// ColorExt(const std::array<Byte, 3> &c)
	//	: ColorExt(B2F(c[0]), B2F(c[1]), B2F(c[2])) {}

	operator Color() const { return Color(F2B(_r), F2B(_g), F2B(_b)); }

	operator std::array<Float, 4>() const {
		return std::array<Float, 4>({_r, _g, _b, _a});
	}

	operator std::array<Float, 3>() const {
		return std::array<Float, 3>({_r, _g, _b});
	}

	// operator std::array<Byte, 3>() const {
	//	return std::array<Byte, 3>({F2B(_r), F2B(_g), F2B(_b)});
	//}

	Float r() const { return _r; }
	Float g() const { return _g; }
	Float b() const { return _b; }
	Float a() const { return _a; }

	// 叠加颜色
	void compose(const ColorExt &c) {
		if (c.a() == 0) {
			_r = c.r();
			_g = c.g();
			_b = c.b();
			_a = 0;
		} else if (c.a() == 1) {
			//
		} else if (_a == 0) {
			_r = c.r() + (_r - c.r()) * c.a();
			_g = c.g() + (_g - c.g()) * c.a();
			_b = c.b() + (_b - c.b()) * c.a();
			_a = 0.0;
		} else if (_a == 1) {
			_r = c.r();
			_g = c.g();
			_b = c.b();
			_a = c.a();
		} else {
			_r = (c.r() * (1.0 - c.a()) + c.a() * _r * (1.0 - _a))
				/ (1.0 - _a * c.a());
			_g = (c.g() * (1.0 - c.a()) + c.a() * _g * (1.0 - _a))
				/ (1.0 - _a * c.a());
			_b = (c.b() * (1.0 - c.a()) + c.a() * _b * (1.0 - _a))
				/ (1.0 - _a * c.a());
			_a = _a * c.a();
		}
	}
};

void compose(ColorExt &a, const ColorExt &b) { a.compose(b); }

class ColorHSV {
	Float _h, _s, _v, _a;
	static Float fit(Float x) { return x < 0 ? 0 : (x > 1 ? 1 : x); }

public:
	ColorHSV() { _h = _s = _v = _a = 0; }
	ColorHSV(const ColorExt &);
	ColorHSV(Float h, Float s, Float v, Float a = 0)
		: _h(fit(h / Const::tau) * Const::tau)
		, _s(fit(s))
		, _v(fit(v))
		, _a(fit(a)) {}

	operator ColorExt() {
		const Float cst = Const::pi / 3;
		int hi = _h / cst;
		Float f, p, q, t;
		f = _h / cst - hi;
		p = _v * (1 - _s);
		q = _v * (1 - f * _s);
		t = _v * (1 - (1 - f) * _s);
		switch (hi) {
		case 0:
			return ColorExt(_v, t, p);
		case 1:
			return ColorExt(q, _v, p);
		case 2:
			return ColorExt(p, _v, t);
		case 3:
			return ColorExt(p, q, _v);
		case 4:
			return ColorExt(t, p, _v);
		case 5:
			return ColorExt(_v, p, q);
		}
	}
};

ColorHSV::ColorHSV(const ColorExt &c) {
	Float r = c.r(), g = c.g(), b = c.b();
	Float cmin, delta;
	const Float cst = Const::pi / 3;
	char vc, mc;
	if (r > g) {
		if (g > b)
			vc = 'r', mc = 'b';
		else if (r > b)
			vc = 'r', mc = 'g';
		else
			vc = 'b', mc = 'g';
	} else {
		if (r > b)
			vc = 'g', mc = 'b';
		else if (g > b)
			vc = 'g', mc = 'r';
		else
			vc = 'b', mc = 'r';
	}
	switch (vc) {
	case 'r':
		_v = r;
		break;
	case 'g':
		_v = g;
		break;
	case 'b':
		_v = b;
		break;
	}
	switch (mc) {
	case 'r':
		cmin = r;
		break;
	case 'g':
		cmin = g;
		break;
	case 'b':
		cmin = b;
		break;
	}
	delta = _v - cmin;
	if (delta > 0) {
		switch (vc) {
		case 'r':
			if (mc == 'b')
				_h = cst * ((g - b) / delta);
			else
				_h = cst * ((g - b) / delta + 6);
		case 'g':
			_h = cst * ((b - r) / delta + 2);
			break;
		case 'b':
			_h = cst * ((r - g) / delta + 4);
			break;
		}
	}
	if (_v > 0) {
		_s = delta / _v;
	}
	_a = c.a();
}
#endif
