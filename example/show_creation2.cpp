#pragma GCC optimize(2)
#include "bmp_old.h"
using namespace std;
const uint Height = 1080;
const uint Width = 1920;
uc BitMap[Height][Width][3];
uc BMPFILE[(Height * Width << 2) + 256];
uc div255[65536];
struct COLOR {
	uc r, g, b, a;
	COLOR(uc r = 0u, uc g = 0u, uc b = 0u, uc a = 0u)
		: r(r)
		, g(g)
		, b(b)
		, a(a) {}
	bool operator==(COLOR c) {
		return r == c.r && g == c.g && b == c.b && a == c.a;
	}
};
namespace FUNCTION {
struct id {
	double operator()(double x) { return x; }
};
struct sqr {
	double operator()(double x) { return x * x; }
};
struct cube {
	double operator()(double x) { return x * x * x; }
};
template <typename FF> struct anti {
	double operator()(double x) {
		FF f;
		return 1 - f(1 - x);
	}
};
double sigmoid(double x) { return 1.0 / (1.0 + exp(-x)); }
struct SMOOTH {
	double operator()(double x, double inflection) {
		double error = sigmoid(-inflection / 2);
		double temp
			= (sigmoid(inflection * (x - 0.5)) - error) / (1 - 2 * error);
		if (temp < 0.0)
			return 0.0;
		if (temp > 1.0)
			return 1.0;
		return temp;
	}
};
struct smooth {
	double operator()(double x) {
		SMOOTH f;
		return f(x, 10.0);
	}
};
template <typename FF> struct sfs {
	double operator()(double x) {
		FF f;
		if (x <= 0.5)
			return 0.5 * f(2.0 * x);
		return 1 - 0.5 * f(2.0 - 2.0 * x);
	}
};
} // namespace FUNCTION
void set_color(uc arr[3], COLOR c) {
	arr[0] = div255[arr[0] * c.a + c.b * (255 - c.a) + 127];
	arr[1] = div255[arr[1] * c.a + c.g * (255 - c.a) + 127];
	arr[2] = div255[arr[2] * c.a + c.r * (255 - c.a) + 127];
}
void clear_all(COLOR bgc = COLOR(0u, 0u, 0u)) {
	for (uint i = 0; i < Height; ++i)
		for (uint j = 0; j < Width; ++j)
			set_color(BitMap[i][j], bgc);
}
struct PIC {
	uint height, width;
	uint size;
	COLOR *pic_file;
	PIC(uint h, uint w)
		: height(h)
		, width(w)
		, size(h * w) {
		pic_file = new COLOR[size];
	}
	PIC(uint h, uint w, COLOR c)
		: height(h)
		, width(w)
		, size(h * w) {
		pic_file = new COLOR[h * w];
		fill(pic_file, pic_file + h * w, c);
	}
	~PIC() { delete[] pic_file; }
	void draw(int bl, int bg) {
		for (uint i = max(-bl, 0); (int)i < min((int)Height - bl, (int)height);
			 ++i) {
			for (uint j = max(-bg, 0);
				 (int)j < min((int)Width - bg, (int)width); ++j) {
				set_color(BitMap[i + bl][j + bg], pic_file[i * width + j]);
			}
		}
	}
	void turn_color(COLOR prev, COLOR cur) {
		for (uint i = 0; i < size; ++i) {
			if (pic_file[i] == prev) {
				pic_file[i] = cur;
			}
		}
	}
	void set_transparent_color(COLOR c) { turn_color(c, COLOR(0, 0, 0, 255)); }
	template <typename FUNC> void show(uint Duration, uint Current_frame) {
		--Duration;
		FUNC f;
		uint alpha = (1.0 - f((double)Current_frame / Duration)) * 255 + 0.5;
		for (uint i = 0; i < size; ++i) {
			pic_file[i].a = alpha;
		}
	}
};

struct FIGURE {
	typedef pair<uint, uint> PIXEL;
	vector<PIXEL> Pixels;
	void draw(int bl, int br, COLOR c, uint num_visible, int th = 1) {
		int half_th = (th >> 1);
		for (uint i = 0; i < num_visible; ++i) {
			for (int j = -half_th; j < th - half_th; ++j)
				for (int k = -half_th; k < th - half_th; ++k) {
					int l = bl + Pixels[i].first + j;
					int r = br + Pixels[i].second + k;
					if (l >= 0 && l < (int)Height && r >= 0 && r < (int)Width)
						set_color(BitMap[l][r], c);
				}
		}
	}
	template <typename FUNC>
	uint Show_Creation(uint Duration, uint Current_frame) {
		FUNC f;
		return f((double)Current_frame / (Duration - 1)) * Pixels.size() + 0.5;
	}
};

template <typename FUNC>
COLOR color_transform(
	COLOR begin, COLOR end, uint Duration, uint Current_frame) {
	--Duration;
	COLOR ans;
	FUNC f;
	ans.r = begin.r + (end.r - begin.r) * f((double)Current_frame / Duration)
		+ 0.5;
	ans.g = begin.g + (end.g - begin.g) * f((double)Current_frame / Duration)
		+ 0.5;
	ans.b = begin.b + (end.r - begin.b) * f((double)Current_frame / Duration)
		+ 0.5;
	return ans;
}
void to_file(const char *filename) {
	uint size = write_bmp(BMPFILE, Height, Width, 24, &BitMap[0][0][0]);
	output_bmp(filename, BMPFILE, size);
}
void draw_rectangle(uint bline, uint brow, uint height, uint width, COLOR c) {
	for (uint i = 0; i < height; ++i)
		for (uint j = 0; j < width; ++j)
			set_color(BitMap[bline + i][brow + j], c);
}
void draw_rectangle(pair<uint, uint> P, uint height, uint width, COLOR c) {
	draw_rectangle(P.first, P.second, height, width, c);
}
template <typename FUNC>
pair<uint, uint> move(pair<uint, uint> begin, pair<uint, uint> end,
	uint Duration, uint Current_frame) {
	--Duration;
	pair<uint, uint> ans;
	FUNC f;
	ans.first = begin.first
		+ (end.first - begin.first) * f((double)Current_frame / Duration) + 0.5;
	ans.second = begin.second
		+ (end.second - begin.second) * f((double)Current_frame / Duration)
		+ 0.5;
	return ans;
}
namespace CONSTANT {
const COLOR RED = COLOR(255, 0, 0);
const COLOR GREEN = COLOR(0, 255, 0);
const COLOR BLUE = COLOR(0, 0, 255);
const COLOR BLACK = COLOR(0, 0, 0);
const COLOR WHITE = COLOR(255, 255, 255);
const COLOR YELLOW = COLOR(255, 255, 0);
const COLOR PURPLE = COLOR(255, 0, 255);
const COLOR CYAN = COLOR(0, 255, 255);
const COLOR TRANSPARENT = COLOR(0, 0, 0, 255);
// const TRANS NORM_TRANS = TRANS(1.0, 0.0, 1.0, 0.0);
} // namespace CONSTANT
using namespace CONSTANT;
int main() {
	system("del Show_Creation_Rectangle2_*.bmp");
	for (uint i = 0; i < 65536; ++i) {
		div255[i] = uc(i / 255);
	}
	clock_t st = clock();
	FIGURE fig;
	for (uint i = 300; i <= 800u; ++i) {
		fig.Pixels.push_back(make_pair(i, 700));
	}
	for (uint i = 700; i <= 1200u; ++i) {
		fig.Pixels.push_back(make_pair(800, i));
	}
	for (uint i = 800; i >= 300u; --i) {
		fig.Pixels.push_back(make_pair(i, 1200));
	}
	for (uint i = 1200; i >= 700u; --i) {
		fig.Pixels.push_back(make_pair(300, i));
	}
	clear_all();
	PIC pic(495u, 495u, GREEN);
	for (uint frame = 0; frame < 45u; ++frame) {
		fig.draw(
			0, 0, WHITE, fig.Show_Creation<FUNCTION::smooth>(45u, frame), 5u);
		char temp[64];
		sprintf(temp, "Show_Creation_Rectangle2_%d.bmp", frame + 1);
		to_file(temp);
		printf("\r%.0lf%%", frame / 0.9);
		fflush(stdout);
		fig.draw(
			0, 0, BLACK, fig.Show_Creation<FUNCTION::smooth>(45u, frame), 5u);
	}
	PIC blk(495u, 495u, BLACK);
	for (uint frame = 45u; frame < 60u; ++frame) {
		fig.draw(0, 0, WHITE, fig.Pixels.size(), 5u);
		pic.show<FUNCTION::smooth>(15u, frame - 45u);
		pic.draw(303, 703);
		char temp[64];
		sprintf(temp, "Show_Creation_Rectangle2_%d.bmp", frame + 1);
		to_file(temp);
		printf("\r%.0lf%%", frame / 0.9);
		fflush(stdout);
		blk.draw(303, 703);
	}
	pic.draw(303, 703);
	for (uint frame = 60u; frame < 90u; ++frame) {
		char temp[64];
		sprintf(temp, "Show_Creation_Rectangle2_%d.bmp", frame + 1);
		to_file(temp);
		printf("\r%.0lf%%", frame / 0.9);
		fflush(stdout);
	}
	// system("ffmpeg -y -i Show_Creation_Rectangle2_%d.bmp -r 30 -pix_fmt
	// yuv420p Show_Creation_Rectangle2.mp4");
	printf("\n%ldms\n", clock() - st);
	return 0;
}
