#ifndef BMP_H
#define BMP_H

#include <bits/stdc++.h>

typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned int Dword;

class BMPFile {

	// 一个 bmp 文件由 header, colormap (可选), img 三部分组成
	Dword header_size;
	Dword colormap_size;
	Dword img_size;
	Dword buf_size; // 等于以上三者之和
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
	Dword *colormap;
	Byte *buf; // 图像文件缓冲区

public:
	BMPFile(Dword height, Dword width, Byte bpp, Dword *colormap = NULL)
		: header_size(sizeof(header) + 2)
		, colormap_size(colormap ? 1u << (bpp + 2) : 0)
		, img_size((((bpp * width + 31) >> 5) << 2) * height)
		, buf_size(header_size + colormap_size + img_size)
		, header()
		, colormap(colormap)
		, buf(new Byte[buf_size]) {
		// bmp header
		header.file_size = buf_size;
		header.reserved1 = 0;
		header.reserved2 = 0;
		header.offbits = header_size + colormap_size;

		// info header
		header.info_size = 40;
		header.width = width;
		header.height = height;
		header.planes = 1;
		header.bit_count = bpp; // bpp: bit per pixel
		header.compression = 0;
		header.img_size = img_size;
		header.resolutionX = 0;
		header.resolutionY = 0;
		header.color_used = 0;
		header.color_important = 0;
	}

	BMPFile(const char *filename)
		: header_size(sizeof(header) + 2)
		, colormap(NULL) {
		FILE *fp = fopen(filename, "r");
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
		buf_size = header.file_size;

		// colormap + img
		buf = new Byte[buf_size];
		if (buf == NULL) {
			std::cerr << "BMPfile: failed to allocate memory\n";
			return;
		}
		memcpy(buf, type, 2);
		memcpy(buf + 2, &header, sizeof(header));
		fread(buf + header_size, sizeof(Byte), buf_size - header_size, fp);

		fclose(fp);
	}

	~BMPFile() {
		if (buf)
			delete[] buf;
		buf = NULL;
	}

	void info() {
		std::cout << "file_size: " << header.file_size
				  << "\nreserved1: " << header.reserved1
				  << "\nreserved2: " << header.reserved2
				  << "\noffbits: " << header.offbits
				  << "\ninfo_size: " << header.info_size
				  << "\nwidth: " << header.width
				  << "\nheight: " << header.height
				  << "\nplanes: " << header.planes
				  << "\nbit_count: " << header.bit_count
				  << "\ncompression: " << header.compression
				  << "\nimg_size: " << header.img_size
				  << "\nresolutionX: " << header.resolutionX
				  << "\nresolutionY: " << header.resolutionY
				  << "\ncolor_used: " << header.color_used
				  << "\ncolor_important: " << header.color_important << "\n";
	}

	Dword write(Byte *img, Dword *colormap = NULL) {

		Byte *ptr = buf;

		// header
		buf[0] = 'B';
		buf[1] = 'M';
		ptr += 2;
		memcpy(ptr, &header, sizeof(header));
		ptr += sizeof(header);

		// colormap
		Byte bpp = header.bit_count;
		if (colormap) {
			Dword size = (1u << bpp) * sizeof(Dword);
			memcpy(ptr, colormap, size);
			ptr += size;
		}

		// img
		if (img) {
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
		}
		return buf_size;
	}

	int output(const char *filename) {
		FILE *fp = fopen(filename, "wb");
		if (!fp) {
			perror(filename);
			return -1;
		}
		fwrite(buf, sizeof(Byte), buf_size, fp);
		fclose(fp);
		return 0;
	}
};

#endif // BMP_H
