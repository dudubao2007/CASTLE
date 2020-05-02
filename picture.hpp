#ifndef PICTURE_HPP
#define PICTURE_HPP
#include "color.hpp"
#include "global_define.hpp"
#include <array>
#include <vector>
template <typename T> struct BasePicture {
	std::vector<std::vector<T>> data;
	BasePicture(Dword _height = 0, Dword _width = 0)
		: data(
			std::vector<std::vector<T>>(_height, std::vector<T>(_width, T()))) {
	}
	template <typename P> operator BasePicture<P>() {
		BasePicture pic(height(), width());
		for (Dword i = 0; i < height(); i++)
			for (Dword j = 0; j < width(); j++)
				pic[i][j] = data[i][j];
	}
	Dword height() const { return data.size(); }
	Dword width() const { return (data.size() > 0 ? data.front().size() : 0); }
};
typedef BasePicture<Color> ColorPicture;
typedef BasePicture<ColorExt> ColorExtPicture;
typedef BasePicture<std::array<Byte, 3>> BytePicture;
typedef std::vector<Dword> ColorMap;
#endif
