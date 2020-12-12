#pragma GCC optimize(2)
#include "bmp_old.hpp"
using namespace std;
const Dword Height = 4096;
const Dword Width = 4096;
Byte BitMap[Height][Width][3];
Byte BMPFILE[4 * Height * Width + 64];
const double xmin = -1.2;
const double xmax = 1.2;
const double ymin = -1.2;
const double ymax = 1.2;
struct Point {
	double x, y;
	Point(double x = 0, double y = 0)
		: x(x)
		, y(y) {}
};
double sdf1[Height][Width], sdf2[Height][Width];
inline Point to_Point(Dword a, Dword b) {
	return Point(xmin + (xmax - xmin) * b / (Width - 1),
		ymin + (ymax - ymin) * a / (Height - 1));
}
const double cst_for_sdf1 = sqrt(4.0 / 3.141592653589793238462643);
void get_sdf1(Dword a, Dword b) {
	Point P = to_Point(a, b);
	double x = P.x, y = P.y;
	sdf1[a][b] = hypot(x, y) - cst_for_sdf1;
}
void get_sdf2(Dword a, Dword b) {
	Point P = to_Point(a, b);
	double x = P.x, y = P.y;
	if (-x <= y && y <= x && -1.0 <= y && y <= 1.0)
		sdf2[a][b] = x - 1.0;
	else if (-y <= x && x <= y && -1.0 <= x && x <= 1.0)
		sdf2[a][b] = y - 1.0;
	else if (x <= y && y <= -x && -1.0 <= y && y <= 1.0)
		sdf2[a][b] = -1.0 - x;
	else if (y <= x && x <= -y && -1.0 <= x && x <= 1.0)
		sdf2[a][b] = -1.0 - y;
	else if (x >= 1.0 && y >= 1.0)
		sdf2[a][b] = hypot(x - 1.0, y - 1.0);
	else if (x <= -1.0 && y >= 1.0)
		sdf2[a][b] = hypot(x + 1.0, y - 1.0);
	else if (x <= -1.0 && y <= -1.0)
		sdf2[a][b] = hypot(x + 1.0, y + 1.0);
	else if (x >= 1.0 && y <= -1.0)
		sdf2[a][b] = hypot(x - 1.0, y + 1.0);
	else
		assert(0);
}
void get_sdf1() {
	for (Dword i = 0; i < Height; ++i)
		for (Dword j = 0; j < Width; ++j)
			get_sdf1(i, j);
}
void get_sdf2() {
	for (Dword i = 0; i < Height; ++i)
		for (Dword j = 0; j < Width; ++j)
			get_sdf2(i, j);
}
double sdf[Height][Width];
void get_sdf() {
	for (Dword i = 0; i < Height; ++i)
		for (Dword j = 0; j < Width; ++j)
			sdf[i][j] = min(
				max(sdf1[i][j], -sdf2[i][j]), max(-sdf1[i][j], sdf2[i][j]));
}
void make_sdf_in_scale() {
	for (Dword i = 0; i < Height; ++i)
		for (Dword j = 0; j < Width; ++j)
			sdf[i][j] = sdf[i][j] / (xmax - xmin) * (Width - 1);
}
struct COLOR {
	Byte r, g, b, a;
	COLOR(Byte r = 0u, Byte g = 0u, Byte b = 0u, Byte a = 0u)
		: r(r)
		, g(g)
		, b(b)
		, a(a) {}
};

void set_color(Byte arr[3], COLOR c) {
	arr[0] = (arr[0] * c.a + c.b * (255 - c.a) + 127) / 255;
	arr[1] = (arr[1] * c.a + c.g * (255 - c.a) + 127) / 255;
	arr[2] = (arr[2] * c.a + c.r * (255 - c.a) + 127) / 255;
}

void RenderSdf(
	COLOR EdgeColor, COLOR InColor, COLOR OutColor, double LineWidth) {
	assert(LineWidth >= 0.0);
	double HLW = 0.5 * LineWidth;
	for (Dword i = 0; i < Height; ++i)
		for (Dword j = 0; j < Width; ++j) {
			if (-HLW <= sdf[i][j] && sdf[i][j] <= HLW)
				set_color(BitMap[i][j], EdgeColor);
			else if (sdf[i][j] < 0.0)
				set_color(BitMap[i][j], InColor);
			else if (sdf[i][j] > 0.0)
				set_color(BitMap[i][j], OutColor);
		}
}
int main() {
	get_sdf1();
	get_sdf2();
	get_sdf();
	make_sdf_in_scale();
	RenderSdf(COLOR(), COLOR(0, 255, 0), COLOR(255, 255, 255), 5.0);
	Dword size = write_bmp(BMPFILE, Height, Width, 24, &BitMap[0][0][0]);
	output_bmp("trial_render_edge_sdf_xor_4096x4096.bmp", BMPFILE, size);
	return 0;
}
