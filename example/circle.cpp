#include "castle.hpp"
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
	Coordinate<Float> center() const { return C; }
	Float radius() const { return r; }
	Float sdf(Coordinate<Float> P) const { return abs(P - C) - r; }
};
template <typename Shape>
void render_shape(ColorExtPicture &pic, const Shape &shape,
	function<ColorExt(Float)> sdf_dealer, Dword thread_num = 1) {
	function<ColorExt(Dword, Dword)> f
		= [&shape, sdf_dealer](Dword i, Dword j) {
			  return sdf_dealer(shape.sdf(Coordinate<Float>(j, i)));
		  };
	Render::render(pic, f, thread_num);
}
template <typename Shape>
void render_shape(ColorExtPicture &pic, const Shape &shape, ColorExt InColor,
	ColorExt EdgeColor, Float LineWidth, Dword thread_num = 1) {
	function<ColorExt(Float)> sdf_dealer
		= [InColor, EdgeColor, LineWidth](Float sdf) {
			  Float halfLW = 0.5 * LineWidth;
			  if (-halfLW < sdf && sdf < halfLW)
				  return EdgeColor;
			  if (sdf < 0)
				  return InColor;
			  return ColorExt(0.0, 0.0, 0.0, 1.0);
		  };
	render_shape(pic, shape, sdf_dealer, thread_num);
}
int main() {
	clock_t start = clock();
	ColorExtPicture pic(1000, 1000);
	CirclePixel circle(500.0, 500.0, 200.0);
	render_shape(
		pic, circle, ColorExt(0.0, 1.0, 0.0), ColorExt(1.0, 1.0, 1.0), 5.0, 4);
	BMPFile bmp(pic);
	bmp.output("circle.bmp");
	printf("Time used:%lfs\n", (double)(clock() - start) / CLOCKS_PER_SEC);
	return 0;
}
