#include "castle.hpp"
#include <bits/stdc++.h>
using namespace std;
class CirclePixel {
	Coordinate<Float> C;
	Float r;

public:
	CirclePixel() {}
	CirclePixel(Coordinate<Float> C, Float r)
		: C(C)
		, r(r) {}
	CirclePixel(Float Cx, Float Cy, Float r)
		: C(Cx, Cy)
		, r(r) {}
	Coordinate<Float> center() { return C; }
	Float radius() { return r; }
	Float sdf(Coordinate<Float> P) { return abs(P - C) - r; }
};
template <typename Shape>
void render(ColorExtPicture &pic, Shape &shape, Float LineWidth,
	ColorExt InColor, ColorExt EdgeColor) {
	Dword height = pic.height(), width = pic.width();
	Float HalfLW = LineWidth * 0.5;
	for (Dword i = 0; i < height; ++i)
		for (Dword j = 0; j < width; ++j) {
			Coordinate<Float> P(j, i);
			Float sdf = shape.sdf(P);
			// cout << sdf << endl;
			if (-HalfLW <= sdf && sdf <= HalfLW) {
				pic.data[i][j] = EdgeColor;
			} else if (sdf < 0) {
				pic.data[i][j] = InColor;
			}
		}
}
int main() {
	CirclePixel Circle(500.0, 500.0, 100.0);
	ColorExtPicture pic(1000, 1000);
	render(pic, Circle, 5, ColorExt(0.0, 1.0, 0.0), ColorExt(1.0, 1.0, 1.0));
	BytePicture BP(pic);
	BMPFile f(BP);
	f.output("circle.bmp");
	return 0;
}
