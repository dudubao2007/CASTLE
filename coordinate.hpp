#ifndef COORDINATE_HPP
#define COORDINATE_HPP
#include "global_define.hpp"
#include <cmath>
#include <complex>
template <typename T>
struct Coordinate {
	T x, y;
	Coordinate(){}
	Coordinate(T x, T y):x(x), y(y){}
	Coordinate(std::complex<T> C):x(C.real()), y(C.imag()){}
	Coordinate<T> operator +(Coordinate<T> P)
	{
		return Coordinate<T>(x+P.x, y+P.y);
	}
	Coordinate<T> operator -(Coordinate<T> P)
	{
		return Coordinate<T>(x-P.x, y-P.y);
	}
	Coordinate<T> operator *(T a)
	{
		return Coordinate<T>(x*a, y*a);
	}
	Coordinate<T> operator /(T a)
	{
		return Coordinate<T>(x/a, y/a);
	}
	operator std::complex<T>()
	{
		return std::complex<T>(x, y);
	}
};
template <typename T>
Coordinate<T> mid(Coordinate<T> P, Coordinate<T> Q)
{
	return (P+Q)*0.5;
}
template <typename T>
T abs(Coordinate<T> P)
{
	return abs(std::complex<T>(P));
}
template <typename T>
T arg(Coordinate<T> P)
{
	return arg(std::complex<T>(P));
}
#endif
