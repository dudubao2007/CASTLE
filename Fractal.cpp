#include "castle.hpp"
#include <bits/stdc++.h>
using namespace std;
Axis axis = Axis(512, 512, 3072, 3072);
Float helper(Point a, Dword n) {
	if (n == 0u) {
		return static_cast<Float>(0.0);
	}
	a.x *= 3.0;
	a.y *= 3.0;
	if (a.x >= 1.0 && a.x <= 2.0 && a.y >= 1.0 && a.y <= 2.0) {
		return static_cast<Float>(
			min(min(a.x - 1.0, 2.0 - a.x), min(a.y - 1.0, 2.0 - a.y)));
	}
	while (a.x >= 1.0)
		a.x -= 1.0;
	while (a.y >= 1.0)
		a.y -= 1.0;
	return helper(a, n - 1u) * 0.5;
}
Float sdf(Point a) {
	if (a.x <= 0.0 || a.x >= 1.0 || a.y <= 0.0 || a.y >= 1.0) {
		if (a.x <= 0.0) {
			if (a.y <= 0.0) {
				return hypot(a.x, a.y);
			}
			if (a.y >= 1.0) {
				return hypot(a.x, a.y - 1.0);
			} else {
				return -a.x;
			}
		} else if (a.x >= 1.0) {
			if (a.y <= 0.0) {
				return hypot(a.x - 1.0, a.y);
			} else if (a.y >= 1.0) {
				return hypot(a.x - 1.0, a.y - 1.0);
			} else {
				return a.x - 1.0;
			}
		} else {
			if (a.y <= 0.0) {
				return -a.y;
			} else if (a.y >= 1.0) {
				return a.y - 1.0;
			} else {
				assert(0);
				return Const::NaN;
			}
		}
	}
	return helper(a, 8);
}
Color renderer(Dword i, Dword j) {
	if (sdf(axis.to_point(i, j)) <= 2.0 / axis.unit())
		return Color(0, 0, 0);
	return Color(255, 255, 255);
}
int main() {
	ColorPicture pic(4096, 4096);
	Render::render(pic, function<Color(Dword, Dword)>(renderer), 4u);
	BMPFile fractal(pic);
	fractal.output("fractal_square.bmp");
	return 0;
}
