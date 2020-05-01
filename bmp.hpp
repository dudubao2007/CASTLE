#ifndef BMP_HPP
#define BMP_HPP

#include "global_define.hpp"
#include "color.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

//一般来说，RGB色深总是24，没有预料过RGB色深不是24的情况
//请注意

class BMPFile {
	// 一个 bmp 文件由 header, colormap (可选), img 三部分组成
	const Dword header_size;
	Dword colormap_size;
	Dword img_size;

	bool new_colormap;
	bool new_img;

	Dword *colormap;
	Byte *img;

public:
	struct {
		// bmp header: 14 - 2 = 12 bytes
		// Byte type[2]; 去掉这一字段, 方便字节对齐
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
	/* BMPFile(BitMap bitmap, Byte bpp = 24,
		Dword *colormap = nullptr) //委托构造函数
		: BMPFile(bitmap.data, bitmap.height(), bitmap.width(), bpp, colormap) {
	} */
	BMPFile(Byte *_img, Dword height, Dword width, Byte bpp = 24,
		Dword *_colormap = NULL)
		: // bpp: bit per pixel
		header_size(sizeof(header) + 2)
		, colormap_size(colormap ? 1 << (bpp + 2) : 0)
		, img_size((((bpp * width + 31) >> 5) << 2) * height)
		, new_colormap(false)
		, new_img(false)
		, colormap(nullptr)
		, img(nullptr) {
		// bmp header
		header.file_size = header_size + colormap_size + img_size;
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

		// copy img&&colormap
		colormap = new Dword[colormap_size];
		img = new Byte[img_size];
		memcpy(colormap, _colormap, sizeof(Dword) * colormap_size);
		memcpy(img, _img, sizeof(Byte) * img_size);
	}

	BMPFile(const char *filename)
		: header_size(sizeof(header) + 2)
		, new_colormap(false)
		, new_img(false)
		, colormap(nullptr)
		, img(nullptr) {
		FILE *fp = fopen(filename, "rb");
		if (!fp) {
			perror(filename);
			return;
		}

		// type
		char type[2];
		fread(type, sizeof(Byte), 2, fp);
		if (type[0] != 'B' || type[1] != 'M') {
			std::cerr << "BMPfile: " << filename << " not start with 'BM'\n";
			return;
		}

		// header
		fread(&header, sizeof(Byte), sizeof(header), fp);
		colormap_size = header.offbits - header_size;
		img_size = header.img_size;

		// colormap
		if (colormap_size) {
			Dword count = colormap_size / sizeof(Dword);
			colormap = new Dword[count];
			if (colormap == NULL) {
				std::cerr
					<< "BMPfile: failed to allocate memory for colormap\n";
				return;
			}
			new_colormap = true;
			fread(colormap, sizeof(Dword), count, fp);
		}

		// img
		if (img_size) {
			img = new Byte[img_size];
			if (img == NULL) {
				std::cerr << "BMPfile: failed to allocate memory for img\n";
				return;
			}
			new_img = true;
			fread(img, sizeof(Byte), img_size, fp);

			// 将数据在内存中移动, 消除空白字节
			Byte bpp = header.bit_count;
			Dword zero_count = (4 - (((bpp >> 3) * header.width) & 3)) & 3;
			if (zero_count) {
				Dword line_count = header.width * (bpp >> 3);
				Dword count = line_count + zero_count;
				for (Byte *r = img + count, *w = img + line_count;
					 r < img + img_size; r += count, w += line_count) {
					memmove(w, r, line_count);
				}
			}
		}

		fclose(fp);
	}

	~BMPFile() {
		if (new_colormap)
			delete[] colormap;
		if (new_img)
			delete[] img;
	}

	int output(const char *filename) const {
		FILE *fp = fopen(filename, "wb");
		if (!fp) {
			perror(filename);
			return -1;
		}

		Byte *buf = new Byte[header.file_size];
		Byte *w = buf;

		// header
		buf[0] = 'B';
		buf[1] = 'M';
		w += 2;
		memcpy(w, &header, sizeof(header));
		w += sizeof(header);

		// colormap
		Byte bpp = header.bit_count;
		if (colormap) {
			memcpy(w, colormap, colormap_size);
			w += colormap_size;
		}

		// img
		if (img) {
			Dword zero_count = (4 - (((bpp >> 3) * header.width) & 3)) & 3;
			Dword line_count = header.width * (bpp >> 3);
			Byte *r = img;
			for (Dword i = 0; i < header.height; ++i) {
				// 从 img 数组拷贝 line_count 个字节
				memcpy(w, r, line_count);
				r += line_count;
				w += line_count;
				// 用零填充 zero_count 个字节
				memset(w, 0, zero_count);
				w += zero_count;
			}
		}
		fwrite(buf, sizeof(Byte), header.file_size, fp);
		fclose(fp);
		delete[] buf;
		return 0;
	}

	Color get(Dword row, Dword col) const {
		Dword idx = (row * header.width + col) * 3;
		return Color {img[idx], img[idx + 1], img[idx + 2]};
	}

	void set(Dword row, Dword col, const Color &color) {
		Dword idx = (row * header.width + col) * 3;
		img[idx] = color.r;
		img[idx + 1] = color.g;
		img[idx + 2] = color.b;
	}
};

#endif // BMP_HPP
