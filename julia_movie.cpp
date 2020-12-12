#pragma GCC optimize(2)
#include "bmp.hpp"
#include <bits/stdc++.h>
using namespace std;
typedef complex<double> Comp;
const Dword Height = 4096;
const Dword Width = 4096;
Byte BitMap[Height][Width][3];
const double cx = -0.163;
const double xmin = -1.5;
const double xmax = 1.5;
const double ymin = -1.5;
const double ymax = 1.5;
struct Point {
	double x, y;
	Point(double x = 0, double y = 0)
		: x(x)
		, y(y) {}
};
inline Point to_Point(Dword a, Dword b) {
	return Point(xmin + (xmax - xmin) * b / (Width - 1),
		ymin + (ymax - ymin) * a / (Height - 1));
}
struct COLOR {
	Byte r, g, b;
	COLOR(Byte r = 0u, Byte g = 0u, Byte b = 0u)
		: r(r)
		, g(g)
		, b(b) {}
};
void set_color(Byte arr[3], COLOR c) {
	arr[0] = c.b;
	arr[1] = c.g;
	arr[2] = c.r;
}
const COLOR bgc = COLOR(255, 255, 255);
struct COLOR_DOUBLE {
	double r, g, b;
	COLOR_DOUBLE(double r = 0.0, double g = 0.0, double b = 0.0)
		: r(r)
		, g(g)
		, b(b) {}
	Byte scale(double x) {
		if (x < 0.0)
			x = -x;
		x -= (int)(0.5 * x) * 2.0;
		if (x > 1.0)
			x = 2.0 - x;
		return x * 255.0 + 0.5;
	}
	operator COLOR() { return COLOR(scale(r), scale(g), scale(b)); }
};
inline COLOR Julia(double x, double y, double a, double b, int n = 100) {
	Comp z = Comp(x, y);
	Comp c = Comp(a, b);
	for (int i = 0; i < n; ++i) {
		z = z * z + c;
		if (abs(max(real(z), imag(z))) > 2.0)
			return bgc;
	}
	return COLOR(COLOR_DOUBLE(real(z), imag(z), abs(z)));
}
void row(Dword s, Dword t) {
	for (Dword i = s; i < t; ++i)
		for (Dword j = 0; j < Width; ++j) {
			Point P = to_Point(i, j);
			double x = P.x, y = P.y;
			set_color(BitMap[i][j], Julia(x, y, -0.163, 0.659, 1000));
		}
}
void process(Dword thread_num = 1) {
	assert(Height % thread_num == 0);
	vector<thread> thd_pool(thread_num);
	Dword p = Height / thread_num;
	for (Dword i = 0; i < thread_num; ++i)
		thd_pool[i] = thread(row, i * p, i * p + p);
	for (Dword i = 0; i < thread_num; ++i)
		thd_pool[i].join();
}
int main() {
	process(4);
	BMPFile bmp(BitMap[0][0], Height, Width);
	bmp.output("Julia_neg_0_163__0_659___4096_4096.bmp");
	return 0;
}
