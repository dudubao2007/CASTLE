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
	Dword height() { return data.size(); }
	Dword width() { return (data.size() > 0 ? data.front().size() : 0); }
};
typedef BasePicture<Color> ColorPicture;
typedef BasePicture<ColorExt> ColorExtPicture;
typedef BasePicture<std::array<Float, 3>> BytePicture;
#endif
