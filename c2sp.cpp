#pragma GCC optimize(2)
#include "bmp_old.h"
#include <thread>
#include <windows.h>
using namespace std;
const uint Height = 1500;
const uint Width = 1500;
uc BitMap[Height][Width][3];
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

void output_video() {
	system(
		"ffmpeg -y -f rawvideo -pix_fmt rgb24 -s 1500x1500 -i "
		"\\\\.\\Pipe\\MyPipe -f mp4 -s 1500x1500 c2sp.mp4 2>ffmpeg_c2sp.log");
}

int main() {
	get_sdf1();
	get_sdf2();
	HANDLE hPipe = CreateNamedPipe("\\\\.\\Pipe\\MyPipe", PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, 0, 0, NMPWAIT_WAIT_FOREVER, 0);
	cout << "Connecting..." << endl;
	thread ffmpeg(output_video);
	if (ConnectNamedPipe(hPipe, NULL) == TRUE) {
		cout << "Connected!" << endl;
	}

	for (frame = 0; frame < 30u; ++frame) {
		COLOR c = COLOR(255, 0, 255);
		COLOR b = COLOR();
		for (uint i = 0; i < Height; ++i)
			for (uint j = 0; j < Width; ++j)
				if (sdf1[i][j] <= 0.0)
					set_color(BitMap[i][j], c);
				else
					set_color(BitMap[i][j], b);
		LPDWORD wlen = 0;
		WriteFile(hPipe, &BitMap[0][0][0], sizeof(BitMap), wlen, 0);
		printf("Frame %d ok.\n", frame + 1);
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
		LPDWORD wlen = 0;
		WriteFile(hPipe, &BitMap[0][0][0], sizeof(BitMap), wlen, 0);
		printf("Frame %d ok.\n", frame + 1);
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
		LPDWORD wlen = 0;
		WriteFile(hPipe, &BitMap[0][0][0], sizeof(BitMap), wlen, 0);
		printf("Frame %d ok.\n", frame + 1);
	}
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	ffmpeg.join();
	return 0;
}
