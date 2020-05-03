#ifndef COORDINATE_HPP
#define COORDINATE_HPP

#include "global_define.hpp"
#include <cmath>
#include <complex>

// 表示平面上的点 (或向量)
template <typename T> struct Coordinate {
	T x, y;

	Coordinate(): x(), y() {} // 用 x,y 的默认构造函数来初始化

	Coordinate(const T &x, const T &y) : x(x) , y(y) {}

	Coordinate(const std::complex<T> &C)
		: x(C.real())
		, y(C.imag()) {}

	operator std::complex<T>() const {
		return std::complex<T>(x, y);
	}

	Coordinate<T> operator+(const Coordinate<T> &P) const {
		return Coordinate<T>(x + P.x, y + P.y);
	}

	Coordinate<T> operator-(const Coordinate<T> &P) const {
		return Coordinate<T>(x - P.x, y - P.y);
	}

	// 数乘
	Coordinate<T> operator*(const T &a) const {
		return Coordinate<T>(x * a, y * a);
	}

	// 与 1/a 作数乘. 当心 a 是整型且 a == 0 的情形
	Coordinate<T> operator/(const T &a) const {
		return Coordinate<T>(x / a, y / a);
	}
};

// 中点
template <typename T>
Coordinate<T> mid(const Coordinate<T> &P, const Coordinate<T> &Q) {
	return (P + Q) * 0.5;
}

// 模; 到原点的距离
template <typename T>
T abs(const Coordinate<T> &P) {
	return abs(std::complex<T>(P));
}

// 辐角
template <typename T>
T arg(const Coordinate<T> &P) {
	return arg(std::complex<T>(P));
}

#endif
