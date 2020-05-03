#ifndef PICTURE_HPP
#define PICTURE_HPP

#include "color.hpp"
#include "global_define.hpp"
#include <array>
#include <vector>

// 图片类的通用模板
template <typename T> struct BasePicture {
	std::vector<std::vector<T>> data;

	BasePicture(Dword height, Dword width)
		: data(
			std::vector<std::vector<T>>(
				height, std::vector<T>(width, T())
			)
		) {}

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

#endif
