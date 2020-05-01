#pragma GCC optimize(2)
#include "bmp_old.h"
using namespace std;
const uint Height = 1500;
const uint Width = 1500;
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
uint frame;
double sdf[Height][Width];
void get_sdf() {
	for (uint i = 0; i < Height; ++i)
		for (uint j = 0; j < Width; ++j)
			sdf[i][j]
				= sdf1[i][j] + (sdf2[i][j] - sdf1[i][j]) * (frame - 30) / 89.0;
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

COLOR get_color() {
	return COLOR(255.0 * (1.0 - (frame - 30) / 89.0) + 0.5,
		255.0 * (frame - 30) / 89.0 + 0.5, 255);
}

int main() {
	get_sdf1();
	get_sdf2();
	for (frame = 0; frame < 30u; ++frame) {
		COLOR c = COLOR(255, 0, 255);
		COLOR b = COLOR();
		for (uint i = 0; i < Height; ++i)
			for (uint j = 0; j < Width; ++j)
				if (sdf1[i][j] <= 0.0)
					set_color(BitMap[i][j], c);
				else
					set_color(BitMap[i][j], b);
		char filename[64];
		sprintf(filename, "circle_to_square%d.bmp", (int)frame + 1);
		uint size = write_bmp(BMPFILE, Height, Width, 24, &BitMap[0][0][0]);
		output_bmp(filename, BMPFILE, size);
	}
	for (frame = 30; frame < 120u; ++frame) {
		get_sdf();
		COLOR c = get_color();
		COLOR b = COLOR();
		for (uint i = 0; i < Height; ++i)
			for (uint j = 0; j < Width; ++j)
				if (sdf[i][j] <= 0.0)
					set_color(BitMap[i][j], c);
				else
					set_color(BitMap[i][j], b);
		char filename[64];
		sprintf(filename, "circle_to_square%d.bmp", (int)frame + 1);
		uint size = write_bmp(BMPFILE, Height, Width, 24, &BitMap[0][0][0]);
		output_bmp(filename, BMPFILE, size);
	}
	for (frame = 120; frame < 150u; ++frame) {
		COLOR c = COLOR(0, 255, 255);
		COLOR b = COLOR();
		for (uint i = 0; i < Height; ++i)
			for (uint j = 0; j < Width; ++j)
				if (sdf2[i][j] <= 0.0)
					set_color(BitMap[i][j], c);
				else
					set_color(BitMap[i][j], b);
		char filename[64];
		sprintf(filename, "circle_to_square%d.bmp", (int)frame + 1);
		uint size = write_bmp(BMPFILE, Height, Width, 24, &BitMap[0][0][0]);
		output_bmp(filename, BMPFILE, size);
	}
	system("ffmpeg -y -i circle_to_square%d.bmp -pix_fmt yuv420p -r 30 "
		   "circle_to_square.mp4");
	return 0;
}
