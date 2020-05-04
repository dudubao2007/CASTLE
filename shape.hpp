#ifndef SHAPE_HPP
#define SHAPE_HPP
#include "coordinate.hpp"
#include "global_define.hpp"
#include <algorithm>
namespace CASTLE {

namespace Shape {
	class Shape {
	protected:
		//坐标位置为左上角（右为x正方向，下为z正方向
		Coordinate<Float> c;
		Shape()
			: c() {}
		Shape(Coordinate<Float> c)
			: c(c) {}
		Shape(Float cx, Float cy)
			: c(cx, cy) {}

	public:
		virtual Float sdf(Coordinate<Float>) const = 0;
		Coordinate<Float> center() const { return c; }
	};
	class Circle : public Shape {
		Float r;

	public:
		Circle()
			: Shape()
			, r() {}
		Circle(Coordinate<Float> c, Float r)
			: Shape(c)
			, r(r) {}
		Circle(Float cx, Float cy, Float r)
			: Shape(cx, cy)
			, r(r) {}
		Float radius() const { return r; }
		Float sdf(Coordinate<Float> p) const { return abs(p - center()) - r; }
	};
	class Rectangle : public Shape {
		Float h, w;

	public:
		Rectangle()
			: Shape()
			, h()
			, w() {}
		Rectangle(Coordinate<Float> c, Float h, Float w)
			: Shape(c)
			, h(h)
			, w(w) {}
		Float height() const { return h; }
		Float width() const { return w; }
		Float sdf(Coordinate<Float> p) const {} //不会写
	};
	class BinaryOp : public Shape {

	protected:
		const Shape &a, &b;
		BinaryOp(const Shape &x, const Shape &y)
			: a(x)
			, b(y) {
			c = Coordinate<Float>((a.center().x + b.center().x) / 2,
				(a.center().y + b.center().y) / 2);
		}
	};
	class Union : public BinaryOp {
	public:
		Union(const Shape &x, const Shape &y)
			: BinaryOp(x, y) {}

		Float sdf(Coordinate<Float> p) const {
			return std::min(a.sdf(p), b.sdf(p));
		}
	};
	class Difference : public BinaryOp {
	public:
		Difference(const Shape &x, const Shape &y)
			: BinaryOp(x, y) {}
		Float sdf(Coordinate<Float> p) const {
			return std::max(a.sdf(p), -b.sdf(p));
		}
	};
	class Intersection : public BinaryOp {
	public:
		Intersection(const Shape &x, const Shape &y)
			: BinaryOp(x, y) {}
		Float sdf(Coordinate<Float> p) const {
			return std::max(a.sdf(p), b.sdf(p));
		}
	};
} // namespace Shape
} // namespace CASTLE
#endif