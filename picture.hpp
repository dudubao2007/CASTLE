#ifndef PICTURE_HPP
#define PICTURE_HPP

#include "color.hpp"
#include "global_define.hpp"
#include <array>
#include <vector>

// 图片类的通用模板
template <typename T> struct BasePicture {
	std::vector<std::vector<T>> data;

	BasePicture(Dword height, Dword width, const T &c = T())
		: data(std::vector<std::vector<T>>(height, std::vector<T>(width, c))) {}

	// 图片类型转换
	template <typename P> operator BasePicture<P>() const {
		BasePicture<P> pic(height(), width());
		for (Dword i = 0; i < height(); i++)
			for (Dword j = 0; j < width(); j++)
				pic.data[i][j] = data[i][j];
		return pic;
	}

	Dword height() const { return data.size(); }

	Dword width() const { return data.size() > 0 ? data.front().size() : 0; }
};

typedef BasePicture<Color> ColorPicture;
typedef BasePicture<ColorExt> ColorExtPicture;
typedef BasePicture<std::array<Byte, 3>> BytePicture;

void compose(ColorExtPicture &a, const ColorExtPicture &b) {
	assert(a.height() == b.height() && a.width() == b.width());
	for (Dword i = 0; i < a.height(); i++)
		for (Dword j = 0; j < a.width(); j++)
			a.data[i][j].compose(b.data[i][j]);
}

#endif
