#ifndef BMP_HPP
#define BMP_HPP

#include "color.hpp"
#include "global_define.hpp"
#include "picture.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

//一般来说，RGB色深总是24，没有预料过RGB色深不是24的情况
//请注意

class BMPFile {
	typedef std::vector<Byte> Img;
	// 一个 bmp 文件由 header, colormap (可选), img 三部分组成
	const Dword header_size;
	Dword colormap_size;
	Dword img_size;

	bool new_colormap;
	bool new_img;

	std::vector<Dword> colormap;
	Img img;
	Img to_img(BytePicture pic) {
		Img res(pic.height() * pic.width() * 3);
		for (Dword i = 0; i < pic.height(); i++)
			for (Dword j = 0; j < pic.width(); j++)
				std::fill(pic.data[i][j].begin(), pic.data[i][j].end(),
					res.begin() + (i * pic.width() + j) * 3);
	}

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
	BMPFile(BytePicture pic, Byte bpp = 24,
		ColorMap _colormap = ColorMap()) //委托构造函数
		: BMPFile(to_img(pic), pic.height(), pic.width(), bpp, colormap) {}

	BMPFile(std::vector<Byte> _img, Dword height, Dword width, Byte bpp = 24,
		ColorMap _colormap = ColorMap())
		: // bpp: bit per pixel
		header_size(sizeof(header) + 2)
		, colormap_size(colormap.size() ? 1 << (bpp + 2) : 0)
		, img_size((((bpp * width + 31) >> 5) << 2) * height)
		, new_colormap(false)
		, new_img(false)
		, colormap(_colormap)
		, img(_img) {
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
	}

	BMPFile(const char *filename)
		: header_size(sizeof(header) + 2)
		, new_colormap(false)
		, new_img(false)
		, colormap(ColorMap())
		, img(Img()) {
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
			try {
				colormap.resize(count);
			} catch (...) {
				std::cerr
					<< "BMPfile: failed to allocate memory for colormap\n";
				return;
			}
			new_colormap = true;
			fread(colormap.data(), sizeof(Dword), count, fp);
		}

		// img
		if (img_size) {
			try {
				img.resize(img_size);
			} catch (...) {
				std::cerr << "BMPfile: failed to allocate memory for img\n";
				return;
			}
			new_img = true;
			fread(img.data(), sizeof(Byte), img_size, fp);

			// 将数据在内存中移动, 消除空白字节
			Byte bpp = header.bit_count;
			Dword zero_count = (4 - (((bpp >> 3) * header.width) & 3)) & 3;
			if (zero_count) {
				Dword line_count = header.width * (bpp >> 3);
				Dword count = line_count + zero_count;
				for (Byte *r = img.data() + count, *w = img.data() + line_count;
					 r < img.data() + img_size; r += count, w += line_count) {
					memmove(w, r, line_count);
				}
			}
		}

		fclose(fp);
	}

	~BMPFile() {}

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
		if (colormap.size()) {
			memcpy(w, colormap.data(), colormap_size);
			w += colormap_size;
		}

		// img
		if (img.size()) {
			Dword zero_count = (4 - (((bpp >> 3) * header.width) & 3)) & 3;
			Dword line_count = header.width * (bpp >> 3);
			Byte *r = const_cast<Byte *>(img.data());
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
	BytePicture to_pic() {
		BytePicture pic(header.height, header.width);
		for (Dword i = 0; i < pic.height(); i++)
			for (Dword j = 0; j < pic.width(); j++)
				std::fill(img.begin() + (i * pic.width() + j) * 3,
					img.begin() + (i * pic.width() + j) * 3 + 3,
					pic.data[i][j].begin());
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
