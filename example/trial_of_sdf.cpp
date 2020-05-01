#pragma GCC optimize(2)
#include "bmp_old.h"
using namespace std;
const uint Height = 4096;
const uint Width = 4096;
uc BitMap[Height][Width][3];
uc BMPFILE[4 * Height * Width + 64];
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
inline Point to_Point(uint a, uint b) {
	return Point(xmin + (xmax - xmin) * b / (Width - 1),
		ymin + (ymax - ymin) * a / (Height - 1));
}
const double cst_for_sdf1 = sqrt(4.0 / 3.141592653589793238462643);
void get_sdf1(uint a, uint b) {
	Point P = to_Point(a, b);
	double x = P.x, y = P.y;
	sdf1[a][b] = hypot(x, y) - cst_for_sdf1;
}
void get_sdf2(uint a, uint b) {
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
	for (uint i = 0; i < Height; ++i)
		for (uint j = 0; j < Width; ++j)
			get_sdf1(i, j);
}
void get_sdf2() {
	for (uint i = 0; i < Height; ++i)
		for (uint j = 0; j < Width; ++j)
			get_sdf2(i, j);
}
double sdf[Height][Width];
void get_sdf() {
	for (uint i = 0; i < Height; ++i)
		for (uint j = 0; j < Width; ++j)
			sdf[i][j] = min(
				max(sdf1[i][j], -sdf2[i][j]), max(-sdf1[i][j], sdf2[i][j]));
}
void make_sdf_in_scale() {
	for (uint i = 0; i < Height; ++i)
		for (uint j = 0; j < Width; ++j)
			sdf[i][j] = sdf[i][j] / (xmax - xmin) * (Width - 1);
}
struct COLOR {
	uc r, g, b, a;
	COLOR(uc r = 0u, uc g = 0u, uc b = 0u, uc a = 0u)
		: r(r)
		, g(g)
		, b(b)
		, a(a) {}
};

void set_color(uc arr[3], COLOR c) {
	arr[0] = (arr[0] * c.a + c.b * (255 - c.a) + 127) / 255;
	arr[1] = (arr[1] * c.a + c.g * (255 - c.a) + 127) / 255;
	arr[2] = (arr[2] * c.a + c.r * (255 - c.a) + 127) / 255;
}

void RenderSdf(
	COLOR EdgeColor, COLOR InColor, COLOR OutColor, double LineWidth) {
	assert(LineWidth >= 0.0);
	double HLW = 0.5 * LineWidth;
	for (uint i = 0; i < Height; ++i)
		for (uint j = 0; j < Width; ++j) {
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
	uint size = write_bmp(BMPFILE, Height, Width, 24, &BitMap[0][0][0]);
	output_bmp("trial_render_edge_sdf_xor_4096x4096.bmp", BMPFILE, size);
	return 0;
}
