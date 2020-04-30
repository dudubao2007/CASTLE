#include <bits/stdc++.h>
#include "write_bmp.hpp"
template <typename T, Dword t>
struct Array {
	T a[t];
	T& operator [](Dword i) {
		return a[i];
	}
	Array(std::complex<T> z) {
		assert(t == 2u);
		a[0] = real(z);
		a[1] = real(z);
	}
	operator std::complex<T>() {
		assert(t == 2u);
		return std::complex<T>(a[0], a[1]);
	}
};
template <typename T, Dword t>
Array<T, t> operator +(Array<T, t> a, Array<T, t> b) {
	Array<T, t> ans;
	for (Dword i = 0; i < t; ++i) {
		ans[i] = a[i] + b[i];
	}
	return ans;
}
template <typename T, Dword t>
Array<T, t> operator -(Array<T, t> a, Array<T, t> b) {
	Array<T, t> ans;
	for (Dword i = 0; i < t; ++i) {
		ans[i] = a[i] - b[i];
	}
	return ans;
}
template <typename T, Dword t>
Array<T, t> operator *(Array<T, t> a, Array<T, t> b) {
	Array<T, t> ans;
	for (Dword i = 0; i < t; ++i) {
		ans[i] = a[i] * b[i];
	}
	return ans;
}
template <typename T, Dword t>
Array<T, t> operator /(Array<T, t> a, Array<T, t> b) {
	Array<T, t> ans;
	for (Dword i = 0; i < t; ++i) {
		ans[i] = a[i] / b[i];
	}
	return ans;
}
template <typename T, Dword t>
Array<T, t> operator +(Array<T, t> a, T b) {
	Array<T, t> ans;
	for (Dword i = 0; i < t; ++i) {
		ans[i] = a[i] + b;
	}
	return ans;
}
template <typename T, Dword t>
Array<T, t> operator -(Array<T, t> a, T b) {
	Array<T, t> ans;
	for (Dword i = 0; i < t; ++i) {
		ans[i] = a[i] - b;
	}
	return ans;
}
template <typename T, Dword t>
Array<T, t> operator *(Array<T, t> a, T b) {
	Array<T, t> ans;
	for (Dword i = 0; i < t; ++i) {
		ans[i] = a[i] * b;
	}
	return ans;
}
template <typename T, Dword t>
Array<T, t> operator /(Array<T, t> a, T b) {
	Array<T, t> ans;
	for (Dword i = 0; i < t; ++i) {
		ans[i] = a[i] / b;
	}
	return ans;
}
template <typename T, Dword t>
Array<T, t> operator +(T a, Array<T, t> b) {
	Array<T, t> ans;
	for (Dword i = 0; i < t; ++i) {
		ans[i] = a + b[i];
	}
	return ans;
}
template <typename T, Dword t>
Array<T, t> operator -(T a, Array<T, t> b) {
	Array<T, t> ans;
	for (Dword i = 0; i < t; ++i) {
		ans[i] = a - b[i];
	}
	return ans;
}
template <typename T, Dword t>
Array<T, t> operator *(T a, Array<T, t> b) {
	Array<T, t> ans;
	for (Dword i = 0; i < t; ++i) {
		ans[i] = a * b[i];
	}
	return ans;
}
template <typename T, Dword t>
Array<T, t> operator /(T a, Array<T, t> b) {
	Array<T, t> ans;
	for (Dword i = 0; i < t; ++i) {
		ans[i] = a / b[i];
	}
	return ans;
}
template <typename T>
T cross(Array<T, 2> a, Array<T, 2> b) {
	return a[0]*b[1] - a[1]*b[0];
}
template <typename T>
Array<T, 3> cross(Array<T, 3> a, Array<T, 3> b) {
	Array<T, 3> ans;
	ans[0] = a[1]*b[2] - a[2]*b[1];
	ans[1] = a[2]*b[0] - a[0]*b[2];
	ans[2] = a[0]*b[1] - a[1]*b[0];
	return ans;
}
//2020-04-30
