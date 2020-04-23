#include <bits/stdc++.h>
typedef unsigned char uc;
typedef unsigned short us;
typedef unsigned int uint;
typedef unsigned long long ull;
namespace BMPUtil
{
void write_bits(uc *ptr, uc num) { (*ptr) = num; }
void write_bits(uc *ptr, uint num)
{
    for (uc *it = ptr, k = 0; k < 4; ++it, ++k) { (*it) = num >> (k * 8); }
}
constexpr uint CalcSize(const uint Height, const uint Width, const uint bpp,
    const uint haveColorMap = 0)
{
    return ((((bpp * (ull)Width) + 31) >> 5) << 2) * Height + 54u
        + (1u << (bpp + 2)) * haveColorMap;
}
}
class BMPFile
{

    std::vector<uc> BMP;
    uint Height, Width;
    uc bpp;

public:
    BMPFile(int Height, uint Width, uc bpp)
        : Height(Height)
        , Width(Width)
        , bpp(bpp)
    {
        BMP.resize(BMPUtil::CalcSize(Height, Width, bpp));
    }
    uint write(uc *BitMap, uint *ColorMap = NULL)
    {
        // FILEHEADER
        using namespace BMPUtil;
        uint Size = CalcSize(Height, Width, bpp, ColorMap ? 1 : 0);

        uc *ptr1 = BMP.data();
        write_bits(ptr1, static_cast<uc>('B'));
        write_bits(ptr1 + 1, static_cast<uc>('M'));
        write_bits(ptr1 + 2, Size);

        write_bits(ptr1 + 6, 0u);

        uint OffBits = 54;
        if (ColorMap) OffBits += (1u << (bpp + 2));
        write_bits(ptr1 + 10, OffBits);

        // INFOHEADER
        uc *ptr2 = ptr1 + 14;
        write_bits(ptr2, 40u);

        write_bits(ptr2 + 4, Width);
        write_bits(ptr2 + 8, Height);

        write_bits(ptr2 + 12, 1u);

        write_bits(ptr2 + 14, bpp);

        write_bits(ptr2 + 16, 0u);
        write_bits(ptr2 + 20,
            static_cast<uint>(
                ((((bpp * (ull)Width) + 31) >> 5) << 2) * Height));
        write_bits(ptr2 + 24, 0u);
        write_bits(ptr2 + 32, 0u);

        // COLORMAP
        uc *ptrm = ptr2 + 40;
        if (ColorMap) {
            uint color_num = (1u << bpp);
            for (uint i = 0; i < color_num; ++i) {
                write_bits(ptrm, ColorMap[i]);
                ptrm += 4;
            }
        }

        // BITMAP
        uint num_zero = (4 - (((bpp >> 3) * Width) & 3)) & 3;
        for (uint i = 0; i < Height; ++i) {
            for (uint j = 0; j < Width; ++j)
                for (uint k = 0u; k < (uint)(bpp >> 3u); ++k) {
                    write_bits(ptrm, *(BitMap++));
                    ++ptrm;
                }
            for (uint j = 0; j < num_zero; ++j) write_bits(ptrm, 0u), ++ptrm;
        }
        return Size;
    }
    int output(const char *filename)
    {
        FILE *fp = fopen(filename, "wb");
        if (!fp) return -1;
        fwrite(BMP.data(), sizeof(uc), BMP.size(), fp);
        fclose(fp);
        return 0;
    }
};
