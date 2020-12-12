#ifndef PICTURE_P_HPP
#define PICTURE_P_HPP

#include "color.hpp"

// 图片类的通用模板
template <Dword _height, Dword _width> struct PictureFixed {
	Byte data[_height][_width][3];
	PictureFixed(const Byte &r = 0, const Byte &g = 0, const Byte &b = 0) {
		for (Dword i = 0; i < _height; ++i)
			for (Dword j = 0; j < _width; ++j) {
				data[i][j][0] = r;
				data[i][j][1] = g;
				data[i][j][2] = b;
			}
	}
	PictureFixed(const Color &c)
		: PictureFixed(c.r, c.g, c.b) {}
	// 图片类型转换

	Dword height() const { return _height; }

	Dword width() const { return _width; }
};

#endif
