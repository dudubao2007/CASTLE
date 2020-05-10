
#ifndef SHAPE_HPP
#define SHAPE_HPP
#include "coordinate.hpp"
#include "global_define.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <initializer_list>
namespace CASTLE {

typedef Coordinate<Float> Point;

namespace Shape {
	class Shape {
	protected:
		//坐标位置为左上角（右为x正方向，下为z正方向
		Point c;
		Shape()
			: c() {}
		Shape(Point c)
			: c(c) {}
		Shape(Float cx, Float cy)
			: c(cx, cy) {}

	public:
		virtual Float sdf(const Point &) const = 0;
		Point transform_center() const { return c; }
	};
	class CirclePixel : public Shape {
		Point c; // 圆心
		Float r; // 半径

	public:
		CirclePixel()
			: Shape()
			, c()
			, r() {}
		CirclePixel(Point c, Float r)
			: Shape(c)
			, c(c)
			, r(r) {}
		CirclePixel(Float cx, Float cy, Float r)
			: Shape(cx, cy)
			, c(cx, cy)
			, r(r) {}
		Float radius() const { return r; }
		Float sdf(const Point &p) const {
			return abs(p - transform_center()) - r;
		}
	};
	/*
	class Rectangle : public Shape {
		Float h, w;

	public:
		Rectangle()
			: Shape()
			, h()
			, w() {}
		Rectangle(Point c, Float h, Float w)
			: Shape(c)
			, h(h)
			, w(w) {}
		Float height() const { return h; }
		Float width() const { return w; }
		Float sdf(const Point &p) const {} //不会写
	};
	*/

	class Line : public Shape {
		Float a, b, c; // a x + b y + c = 0

	public:
		Line(const Float &a, const Float &b, const Float &c)
			: a(a)
			, b(b)
			, c(c) {}

		Line(const Point &p1, const Point &p2)
			: a(p2.y - p1.y)
			, b(p1.x - p2.x)
			, c(p2.x * p1.y - p1.x * p2.y) {}

		Float sdf(const Point &P) const {
			return ::abs(a * P.x + b * P.y + c) / hypot(a, b);
		}
	};

	class Segment : public Shape {
		Point p1, p2;

	public:
		Segment(const Point &p1, const Point &p2)
			: Shape(mid(p1, p2))
			, p1(p1)
			, p2(p2) {}

		Float sdf(const Point &p) const {
			Point edge = p1 - p2, r1 = p1 - p, r2 = p2 - p;
			if (edge.x * r1.x + edge.y * r1.y <= 0) // p p1 p2 是钝角
				return abs(r1);
			if (edge.x * r2.x + edge.y * r2.y >= 0) // p p2 p1 是钝角
				return abs(r2);
			return ::abs(r1.x * edge.y - edge.x * r1.y) / abs(edge);
		}
	};

	class Polygon : public Shape {
		std::vector<Point> vertices; // 沿边界逆时针方向
		enum PolygonMode { ALL_INSIDE, ALL_OUTSIDE, EVEN_ODD, NON_ZERO };
		PolygonMode mode;

	public:
		Polygon(const std::vector<Point> &v)
			: vertices(v)
			, mode(EVEN_ODD) {}

		Polygon(const std::initializer_list<Point> &l)
			: vertices(l)
			, mode(EVEN_ODD) {}

		Float sdf(const Point &P) const {
			if (vertices.empty())
				std::cerr << "a polygon must have at least one vertex\n";
			if (vertices.size() == 1)
				return abs(vertices.front() - P);

			// 求到每一边的最短距离
			Float dist = Segment(vertices.front(), vertices.back()).sdf(P);
			for (size_t i = 1; i < vertices.size(); ++i) {
				Float tmp = Segment(vertices[i - 1], vertices[i]).sdf(P);
				if (tmp < dist)
					dist = tmp;
			}

			if (mode == ALL_INSIDE)
				return -dist;
			if (mode == ALL_OUTSIDE)
				return dist;

			// 向径
			std::vector<Point> radius(vertices.size());
			for (size_t i = 0; i < radius.size(); ++i) {
				radius[i] = vertices[i] - P;
			}

			// 求转过的角度 TODO: 比较费时, 待优化
			Float total_angle = angle(radius.back(), radius.front());
			for (size_t i = 1; i < vertices.size(); ++i) {
				total_angle += angle(radius[i - 1], radius[i]);
			}

			// winding number = 角度 / 2pi
			int w = int(round(total_angle /= Const::tau));

			if (mode == EVEN_ODD)
				return w % 2 == 0 ? dist : -dist;
			else // (mode == NON_ZERO)
				return w == 0 ? dist : -dist;
		}
	};

	class BinaryOp : public Shape {

	protected:
		const Shape &a, &b;
		BinaryOp(const Shape &x, const Shape &y)
			: a(x)
			, b(y) {
			c = Point((a.transform_center().x + b.transform_center().x) / 2,
				(a.transform_center().y + b.transform_center().y) / 2);
		}
	};
	class Union : public BinaryOp {
	public:
		Union(const Shape &x, const Shape &y)
			: BinaryOp(x, y) {}

		Float sdf(Point p) const { return std::min(a.sdf(p), b.sdf(p)); }
	};
	class Difference : public BinaryOp {
	public:
		Difference(const Shape &x, const Shape &y)
			: BinaryOp(x, y) {}
		Float sdf(Point p) const { return std::max(a.sdf(p), -b.sdf(p)); }
	};
	class Intersection : public BinaryOp {
	public:
		Intersection(const Shape &x, const Shape &y)
			: BinaryOp(x, y) {}
		Float sdf(Point p) const { return std::max(a.sdf(p), b.sdf(p)); }
	};
} // namespace Shape
} // namespace CASTLE
#endif