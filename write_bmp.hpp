#ifndef WRITE_BMP_H
#define WRITE_BMP_H

#include <bits/stdc++.h>

typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned int Dword;
typedef unsigned long long Ull;

class BMPFile {

	// 一个 bmp 文件由 header, colormap (可选), img 三部分组成
	const Dword header_size;
	const Dword colormap_size;
	const Dword img_size;
	const Dword buf_size; // 等于以上三者之和
	struct {
		// bmp header: 14 bytes
		Byte type[2];
		Dword file_size;
		Word reserved1;
		Word reserved2;
		Dword offbits;

		// info header: 40 bytes
		Dword info_size;
		Dword width;
		Dword height;
		Word planes;
		Word bit_count;
		Dword compression;
		Dword img_size;
		Dword resolutionX; // 单位: pixel/meter
		Dword resolutionY;
		Dword color_used;
		Dword color_important;
	} header;
	Dword *colormap;
	Byte *buf; // 图像文件缓冲区
	Byte *ptr; // 当前读写指针

private:
	template<typename T>
	void write_bits(T data) {
		for (Dword i = 0; i < sizeof(T); ++i) {
			*ptr = data >> (i << 3);
			++ptr;
		}
	}

public:
		BMPFile(Dword height, Dword width, Byte bpp, Dword *colormap=NULL):
		header_size(54),
		colormap_size(colormap ? 1u << (bpp+2) : 0),
		img_size(((((bpp*(Ull)width) + 31) >> 5) << 2) * height),
		buf_size(header_size + colormap_size + img_size),
		header(), colormap(colormap), buf(new Byte[buf_size]), ptr(buf)
	{
		// bmp header
		header.type[0] = 'B';
		header.type[1] = 'M';
		header.file_size = buf_size;
		header.reserved1 = 0;
		header.reserved2 = 0;
		header.offbits = header_size + colormap_size;

		// info header
		header.info_size = 40;
		header.width = width;
		header.height = height;
		header.planes = 1;
		header.bit_count = bpp;
		header.compression = 0;
		header.img_size = img_size;
		header.resolutionX = 0;
		header.resolutionY = 0;
		header.color_used = 0;
		header.color_important = 0;
	}

	~BMPFile() {
		delete[] buf;
	}

	Dword write(Byte *img, Dword *colormap = NULL) {
		// file header
		write_bits(header.type[0]);
		write_bits(header.type[1]);

		write_bits(header.file_size);
		write_bits(header.reserved1);
		write_bits(header.reserved2);
		write_bits(header.offbits);

		// info header
		write_bits(header.info_size);
		write_bits(header.width);
		write_bits(header.height);
		write_bits(header.planes);
		write_bits(header.bit_count);
		write_bits(header.compression);
		write_bits(header.img_size);
		write_bits(header.resolutionX);
		write_bits(header.resolutionY);
		write_bits(header.color_used);
		write_bits(header.color_important);

		// colormap
		Byte bpp = header.bit_count;
		if (colormap) {
			Dword color_count = (1u << bpp);
			for (Dword i = 0; i < color_count; ++i)
				write_bits(colormap[i]);
		}

		// img
		Dword line_count = header.width * (bpp >> 3u);
		Dword zero_count = (4 - (((bpp >> 3) * header.width) & 3)) & 3;
		for (Dword i = 0; i < header.height; ++i) {
			// 从 img 数组拷贝 line_count 个字节
			memcpy(ptr, img, line_count);
			img += line_count;
			ptr += line_count;
			// 用零填充 zero_count 个字节
			memset(ptr, 0, zero_count);
			ptr += zero_count;
		}
		return buf_size;
	}

	int output(const char *filename) {
		FILE *fp = fopen(filename, "wb");
		if (!fp) {
			perror("failed to open file");
			return -1;
		}
		fwrite(buf, sizeof(Byte), buf_size, fp);
		fclose(fp);
		return 0;
	}
};

#endif // WRITE_BMP_H
