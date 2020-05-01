#include "bmp_old.hpp"

const Dword Height = 4096u;
const Dword Width = 4096u;

Dword Func(Dword a) { return a * a + 111234567u * a + 212433u; }

Byte BitMap[Height][Width][3];

void write_in(Dword i, Dword j) {
	Dword t = Func(i ^ j);
	BitMap[i][j][0] = (t & 255u);
	t >>= 8u;
	BitMap[i][j][1] = (t & 255u);
	t >>= 8u;
	BitMap[i][j][2] = (t & 255u);
}

int main() {
	for (Dword i = 0u; i < Height; ++i)
		for (Dword j = 0u; j < Width; ++j)
			write_in(i, j);

	BMPFile f(Height, Width, 24u);
	f.write(&BitMap[0][0][0]);
	f.output("xor.bmp");

	return 0;
}
