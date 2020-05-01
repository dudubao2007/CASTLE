#include "array.hpp"
#include "global_define.hpp"
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
	Byte F2B(Float x) {
		if (x <= 0.0)
			return 0;
		if (x >= 1.0)
			return 255;
		return Byte(x * 256);
	}
	Float B2F(Byte x) { return x /static_cast<Float>(255.0); }

public:
	ColorExt() {}
	ColorExt(Float r, Float g, Float b, Float a = 0)
		: _r(r)
		, _g(g)
		, _b(b)
		, _a(a) {
		auto compress
			= [](Float &x) { return x = (x < 0) ? 0.0 : ((x > 1) ? 1.0 : x); };
		compress(_r);
		compress(_g);
		compress(_b);
		compress(_a);
	}
	ColorExt(Array<Float, 4> c)
		: ColorExt(c[0],c[1],c[2],c[3]){
		
	}
	ColorExt(Array<Float, 3> c)
		: ColorExt(c[0],c[1],c[2]){
	}
	ColorExt(Color c)
		: ColorExt(c.r,c.g,c.b){}
	operator Color() { return Color(F2B(_r), F2B(_g), F2B(_b)); }
	operator Array<Float, 4>() {
		Array<Float, 4> ans;
		ans[0] = _r;
		ans[1] = _g;
		ans[2] = _b;
		ans[3] = _a;
		return ans;
	}
	operator Array<Float, 3>() {
		Array<Float, 3> ans;
		ans[0] = _r;
		ans[1] = _g;
		ans[2] = _b;
		return ans;
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
