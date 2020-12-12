#include "castle.hpp"
#include <bits/stdc++.h>
using namespace std;
Axis axis = Axis(2048, 2048, 1500, 1500);
Color renderer(Dword i, Dword j) {
	if (abs(axis.to_point(i, j)) <= 1.0)
		return Color(0, 0, 0);
	return Color(255, 255, 255);
}
int main() {
	ColorPicture pic(4096, 4096);
	Render::render(pic, function<Color(Dword, Dword)>(renderer), 4u);
	BMPFile bmp(pic);
	bmp.output("simple_circle.bmp");
	return 0;
}
