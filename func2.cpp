//#pragma GCC optimize(2)
#include "castle.hpp"
#include <bits/stdc++.h>
using namespace std;
ColorPicture pic(1080, 1920, Color(255, 255, 255));
Color color(0, 0, 0);
Axis axis(960, 540, 530);
Float func(Float x) { return x * x; }
Dword linewidth = 5;
int main() {
	Dword prev = UINT_MAX - 1;
	Dword width = pic.width(), height = pic.height();
	Float x;
	Float py;
	Dword pix_y, l, r;
	for (Dword i = 0; i < width; ++i) {
		x = axis.to_x(i);
		py = axis.to_py(func(x));
		if (py != py)
			pix_y = UINT_MAX - 1;
		else if (py >= height)
			pix_y = height;
		else if (py < 0)
			pix_y = UINT_MAX;
		else
			pix_y = Dword(py);
		if (pix_y < height) {
			if (prev == UINT_MAX - 1 || pix_y == prev)
				l = r = pix_y;
			else if (pix_y + 1 > prev + 1)
				l = prev + 1, r = pix_y;
			else
				l = pix_y, r = prev - 1;
			if (l >= (linewidth - 1) / 2)
				l -= (linewidth - 1) / 2;
			else
				l = 0;
			r += linewidth / 2;
			if (r >= height)
				r = height - 1;
		} else
			l = UINT_MAX, r = 0;
		for (Dword j = l; j <= r; ++j) {
			pic.data[j][i] = color;
		}
		prev = pix_y;
	}
	BMPFile bmp(pic);
	bmp.output("func2.bmp");
	return 0;
}
