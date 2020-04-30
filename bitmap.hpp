#include "global_define.hpp"
template <Dword _height, Dword _width>
struct BitMap {
    Byte data[_height][_width][3];
    Dword height() { return _height; }
    Dword width() { return _width; }
};