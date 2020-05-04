#include "castle.hpp"
#include "time_stamp.hpp"
#include <initializer_list>
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

typedef Coordinate<Float> Point;
class Line {
	Float a, b, c; // a x + b y + c = 0

public:
	Line(const Float &a, const Float &b, const Float &c):
		a(a), b(b), c(c) {}

	Line(const Point &p1, const Point &p2):
		a(p2.y-p1.y), b(p1.x-p2.x), c(p2.x*p1.y-p1.x*p2.y) {}

	Float sdf(const Point &P) const {
		return abs(a * P.x + b * P.y + c)/hypot(a, b);
	}
};

class Segment {
	Point p1, p2;

public:
	Segment(const Point &p1, const Point &p2): p1(p1), p2(p2) {}

	Float sdf(const Point &p) const {
		Point edge = p1 - p2, r1 = p1 - p, r2 = p2 - p;
		if (edge.x * r1.x + edge.y * r1.y <= 0) // p p1 p2 是钝角
			return abs(r1);
		if (edge.x * r2.x + edge.y * r2.y >= 0) // p p2 p1 是钝角
			return abs(r2);
		return abs(r1.x * edge.y - edge.x * r1.y) / abs(edge);
	}
};

class Polygon {
	vector<Point> vertices; // 沿边界逆时针方向

public:
	Polygon(const vector<Point> &v)
		: vertices(v) {}

	Polygon(const initializer_list<Point> &l)
		: vertices(l) {}

	Float sdf(const Point &P) const {
		if (vertices.empty())
			cerr << "a polygon must have at least one vertex\n";
		if (vertices.size() == 1)
			return abs(vertices.front() - P);
		// 求到每一边的最短距离
		Float dist = Segment(vertices.front(), vertices.back()).sdf(P);
		for (size_t i = 1; i < vertices.size(); ++i) {
			Float tmp = Segment(vertices[i-1], vertices[i]).sdf(P);
			if (tmp < dist)
				dist = tmp;
		}

		vector<Point> radius(vertices.size());
		for (size_t i = 0; i < radius.size(); ++i) {
			radius[i] = vertices[i] - P;
		}

		Float winding_number = angle(radius.back(), radius.front());
		for (size_t i = 1; i < vertices.size(); ++i) {
			winding_number += angle(radius[i-1], radius[i]);
		}
		winding_number /= Const::tau;
		return int(round(winding_number)) % 2 == 0 ? dist : -dist;
	}
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
	TimeStamp _("Render");
	ColorExtPicture pic(1000, 1000);
	//Segment shape(Point(400, 400), Point(600, 600));
	//Polygon shape({Point(400,400), Point(600,400), Point(600,600), Point(400,600)});
	Polygon shape({Point(400, 400), Point(600, 600), Point(600, 800), Point(700, 700), Point(200, 700)});
	render_shape(
		pic, shape, ColorExt(0.0, 1.0, 0.0), ColorExt(1.0, 1.0, 1.0), 10.0, 4);
	BMPFile bmp(pic);
	bmp.output("test.bmp");
	return 0;
}
