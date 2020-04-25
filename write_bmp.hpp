#include <bits/stdc++.h>
typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned int Dword;
typedef unsigned long long Ull;
namespace BMPUtil {
void write_bits(Byte *ptr, Byte num) {
	(*ptr) = num;
}
void write_bits(Byte *ptr, Dword num) {
    for (Byte *it = ptr, k = 0; k < 4; ++it, ++k) { (*it) = num >> (k * 8); }
}
constexpr Dword CalcSize(const Dword Height, const Dword Width, const Dword bpp,
    const Dword haveColorMap = 0) {
    return ((((bpp * (Ull)Width) + 31) >> 5) << 2) * Height + 54u
        + (1u << (bpp + 2)) * haveColorMap;
}
}
class BMPFile {

    std::vector<Byte> BMP;
    Dword Height, Width;
    Byte bpp;

public:
    BMPFile(int Height, Dword Width, Byte bpp)
        : Height(Height)
        , Width(Width)
        , bpp(bpp) {
        BMP.resize(BMPUtil::CalcSize(Height, Width, bpp));
    }
    Dword write(Byte *BitMap, Dword *ColorMap = NULL) {
        // FILEHEADER
        using namespace BMPUtil;
        Dword Size = CalcSize(Height, Width, bpp, ColorMap ? 1 : 0);

        Byte *ptr1 = BMP.data();
        write_bits(ptr1, static_cast<Byte>('B'));
        write_bits(ptr1 + 1, static_cast<Byte>('M'));
        write_bits(ptr1 + 2, Size);

        write_bits(ptr1 + 6, 0u);

        Dword OffBits = 54;
        if (ColorMap) OffBits += (1u << (bpp + 2));
        write_bits(ptr1 + 10, OffBits);

        // INFOHEADER
        Byte *ptr2 = ptr1 + 14;
        write_bits(ptr2, 40u);

        write_bits(ptr2 + 4, Width);
        write_bits(ptr2 + 8, Height);

        write_bits(ptr2 + 12, 1u);

        write_bits(ptr2 + 14, bpp);

        write_bits(ptr2 + 16, 0u);
        write_bits(ptr2 + 20,
            static_cast<Dword>(
                ((((bpp * (Ull)Width) + 31) >> 5) << 2) * Height));
        write_bits(ptr2 + 24, 0u);
        write_bits(ptr2 + 32, 0u);

        // COLORMAP
        Byte *ptrm = ptr2 + 40;
        if (ColorMap) {
            Dword color_num = (1u << bpp);
            for (Dword i = 0; i < color_num; ++i) {
                write_bits(ptrm, ColorMap[i]);
                ptrm += 4;
            }
        }

        // BITMAP
        Dword num_zero = (4 - (((bpp >> 3) * Width) & 3)) & 3;
        for (Dword i = 0; i < Height; ++i) {
            for (Dword j = 0; j < Width; ++j)
                for (Dword k = 0u; k < (Dword)(bpp >> 3u); ++k) {
                    write_bits(ptrm, *(BitMap++));
                    ++ptrm;
                }
            for (Dword j = 0; j < num_zero; ++j) write_bits(ptrm, 0u), ++ptrm;
        }
        return Size;
    }
    int output(const char *filename) {
        FILE *fp = fopen(filename, "wb");
        if (!fp) return -1;
        fwrite(BMP.data(), sizeof(Byte), BMP.size(), fp);
        fclose(fp);
        return 0;
    }
};
