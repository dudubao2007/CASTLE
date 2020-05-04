#include "castle.hpp"
#include "shape.hpp"
#include "time_stamp.hpp"
using namespace std;
/*
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
*/

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
	TimeStamp _("Render");
	ColorExtPicture pic(1000, 1000);
	using namespace CASTLE::Shape;
	using namespace CASTLE;
	CirclePixel shape(Point(500, 500), 200);
	// Segment shape(Point(400, 400), Point(600, 600));
	// Polygon shape({Point(400,400), Point(600,400), Point(600,600),
	// Point(400,600)});
	// Polygon shape({Point(400, 400), Point(600, 600), Point(600, 800),
	// Point(700, 700), Point(200, 700)});
	render_shape(
		pic, shape, ColorExt(0.0, 1.0, 0.0), ColorExt(1.0, 1.0, 1.0), 10.0, 4);
	BMPFile bmp(pic);
	bmp.output("test.bmp");
	return 0;
}
