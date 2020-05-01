#include "global_define.hpp"
#include <array>
#include <vector>
struct BitMap {
	std::vector<std::vector<std::array<Byte, 3>>> data;
	BitMap(Dword _height = 0, Dword _width = 0)
		: data(
			std::vector(_height, std::vector(_width, std::array<Byte, 3>()))) {}
	Dword height() { return data.size(); }
	Dword width() { return (data.size() > 0 ? data.front().size() : 0); }
};