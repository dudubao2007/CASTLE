#pragma GCC optimize(2)
#include "bmp_old.h"
using namespace std;
typedef complex<double> Comp;
const uint Height = 4096;
const uint Width = 4096;
uc BitMap[Height][Width][3];
uc BMPFILE[4 * Height * Width + 64];
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
inline Point to_Point(uint a, uint b) {
	return Point(xmin + (xmax - xmin) * b / (Width - 1),
		ymin + (ymax - ymin) * a / (Height - 1));
}
struct COLOR {
	uc r, g, b;
	COLOR(uc r = 0u, uc g = 0u, uc b = 0u)
		: r(r)
		, g(g)
		, b(b) {}
};
void set_color(uc arr[3], COLOR c) {
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
	uc scale(double x) {
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
void row(uint s, uint t) {
	for (uint i = s; i < t; ++i)
		for (uint j = 0; j < Width; ++j) {
			Point P = to_Point(i, j);
			double x = P.x, y = P.y;
			set_color(BitMap[i][j], Julia(x, y, -0.163, 0.659, 1000));
		}
}
void process(uint thread_num = 1) {
	assert(Height % thread_num == 0);
	vector<thread> thd_pool(thread_num);
	uint p = Height / thread_num;
	for (uint i = 0; i < thread_num; ++i)
		thd_pool[i] = thread(row, i * p, i * p + p);
	for (uint i = 0; i < thread_num; ++i)
		thd_pool[i].join();
}
int main() {
	process(4);
	uint size = write_bmp(BMPFILE, Height, Width, 24, &BitMap[0][0][0]);
	output_bmp("Julia_neg_0_163__0_659___4096_4096.bmp", BMPFILE, size);
	return 0;
}
