#include <bits/stdc++.h>
typedef unsigned char uc;
typedef unsigned short us;
typedef unsigned int uint;
typedef unsigned long long ull;
template <typename INT> void write_bits(uc *begin, uc *end, uc num) {
	for (uc *it = begin; it < end; ++it) {
		(*it) = num;
	}
}
void write_bits(uc *begin, uc *end, uint num) {
	for (uc *it = begin, k = 0; it < end; ++it, ++k) {
		(*it) = num >> (k * 8);
	}
}
uint write_bmp(uc *res, uint Height, uint Width, uc bpp, uc *BitMap,
	uint *ColorMap = NULL) {
	// FILEHEADER
	uc *ptr1 = res;
	write_bits(ptr1, ptr1 + 1, 'B');
	write_bits(ptr1 + 1, ptr1 + 2, 'M');

	uint Size = ((((bpp * (ull)Width) + 31) >> 5) << 2) * Height + 54u;
	if (ColorMap)
		Size += (1u << (bpp + 2));
	write_bits(ptr1 + 2, ptr1 + 6, Size);

	write_bits(ptr1 + 6, ptr1 + 10, 0u);

	uint OffBits = 54;
	if (ColorMap)
		OffBits += (1u << (bpp + 2));
	write_bits(ptr1 + 10, ptr1 + 14, OffBits);

	// INFOHEADER
	uc *ptr2 = ptr1 + 14;
	write_bits(ptr2, ptr2 + 4, 40);

	write_bits(ptr2 + 4, ptr2 + 8, Width);
	write_bits(ptr2 + 8, ptr2 + 12, Height);

	write_bits(ptr2 + 12, ptr2 + 14, 1u);

	write_bits(ptr2 + 14, ptr2 + 16, bpp);

	write_bits(ptr2 + 16, ptr2 + 20, 0u);
	write_bits(
		ptr2 + 20, ptr2 + 24, ((((bpp * (ull)Width) + 31) >> 5) << 2) * Height);
	write_bits(ptr2 + 24, ptr2 + 32, 0ull);
	write_bits(ptr2 + 32, ptr2 + 40, 0ull);

	// COLORMAP
	uc *ptrm = ptr2 + 40;
	if (ColorMap) {
		uint color_num = (1u << bpp);
		for (uint i = 0; i < color_num; ++i) {
			write_bits(ptrm, ptrm + 4, ColorMap[i]);
			ptrm += 4;
		}
	}

	// BITMAP
	uint num_zero = (4 - (((bpp >> 3) * Width) & 3)) & 3;
	for (uint i = 0; i < Height; ++i) {
		for (uint j = 0; j < Width; ++j)
			for (uint k = 0u; k < (uint)(bpp >> 3u); ++k) {
				write_bits(ptrm, ptrm + 1, *(BitMap++));
				++ptrm;
			}
		for (uint j = 0; j < num_zero; ++j)
			write_bits(ptrm, ptrm + 1, 0u), ++ptrm;
	}
	return Size;
}
int output_bmp(const char *filename, uc *bmp_str, uint size) {
	FILE *fp = fopen(filename, "wb");
	if (!fp)
		return -1;
	fwrite(bmp_str, sizeof(uc), size, fp);
	fclose(fp);
	return 0;
}
