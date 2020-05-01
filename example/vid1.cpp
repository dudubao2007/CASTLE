#pragma GCC optimize(2)
#include "bmp_old.h"
using namespace std;
uint ColorMap[256] = {16777215, 255};
const uint N = 1020;
const double zoom = pow(1000.0, 1.0 / (N - 1));
double ZOOM = 1.0;
const uint Height = 2048;
const uint Width = 2048;
uc BitMap[Height][Width];
uc BMPFILE[Height * Width + 2048];
double xmin = -5.0, xmax = 5.0, ymin = -1.1, ymax = 1.1;
uint frame;
void update() {
	xmin = -5.0 / ZOOM;
	xmax = 5.0 / ZOOM;
}
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
inline uc get_color(double x, double y) {
	if (y < sin(1.0 / x))
		return 1;
	return 0;
}

void row(uint s, uint t) {
	for (uint i = s; i < t; ++i)
		for (uint j = 0; j < Width; ++j) {
			Point P = to_Point(i, j);
			BitMap[i][j] = get_color(P.x, P.y);
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
	for (frame = 1; frame <= N; ++frame) {
		update();
		process(4);
		uint size
			= write_bmp(BMPFILE, Height, Width, 8, &BitMap[0][0], ColorMap);
		char filename[128];
		sprintf(filename, "Vid1_%d.bmp", frame);
		output_bmp(filename, BMPFILE, size);
		printf("Vid1_%d.bmp Done!\n", frame);
		ZOOM *= zoom;
	}
	return 0;
}
