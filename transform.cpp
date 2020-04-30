#pragma GCC optimize(2)
#include "bmp_old.h"
using namespace std;
const uint Height = 1080;
const uint Width = 1920;
uc BitMap[Height][Width][3];
uc BMPFILE[(Height * Width << 2) + 256];
struct PIXEL
{
	uint line_num, row_num;
	PIXEL (uint l = 0, uint r = 0):line_num(l), row_num(r){}
	bool operator ==(PIXEL a)
	{
		return a.line_num == line_num && a.row_num == row_num;
	}
	bool operator <(PIXEL a)
	{
		return line_num < a.line_num || (line_num == a.line_num && row_num < a.row_num);
	}
};
struct COOR
{
	double x, y;
	COOR (double x = 0, double y = 0):x(x), y(y){}
	//x = ax * row_num + bx
	//y = ay * line_num + by
};

struct TRANS
{
	double ax, bx, ay, by;
	TRANS (double ax, double bx, double ay, double by):ax(ax), bx(bx), ay(ay), by(by){}
	COOR P2C(PIXEL P)
	{
		return COOR(ax * P.row_num + bx, ay * P.line_num + by);
	}
	PIXEL C2P(COOR C)
	{
		return PIXEL((C.y - by) / ay + 0.5, (C.x - bx) / ax + 0.5);
	}
};

struct COLOR
{
	uc r, g, b, a;
	COLOR(uc r = 0u, uc g = 0u, uc b = 0u, uc a = 0u):r(r), g(g), b(b), a(a){}
	bool operator ==(COLOR c)
	{
		return r == c.r && g == c.g && b == c.b && a == c.a;
	}
};

namespace FUNCTION
{
struct id
{
	double operator ()(double x)
	{
		return x;
	}
};
struct sqr
{
	double operator ()(double x)
	{
		return x * x;
	}
};
struct cube
{
	double operator ()(double x)
	{
		return x * x * x;
	}
};
template<typename FF>
struct anti
{
	double operator ()(double x)
	{
		FF f;
		return 1 - f(1 - x);
	}
};
double sigmoid(double x)
{
	return 1.0 / (1.0 + exp(-x));
}
struct SMOOTH
{
	double operator ()(double x, double inflection)
	{
		double error = sigmoid(-inflection / 2);
		double temp = (sigmoid(inflection * (x - 0.5)) - error) / (1 - 2 * error);
		if (temp < 0.0) return 0.0;
		if (temp > 1.0) return 1.0;
		return temp;
	}
};
struct smooth
{
	double operator ()(double x)
	{
		SMOOTH f;
		return f(x, 10.0);
	}
};
template<typename FF>
struct sfs
{
	double operator ()(double x)
	{
		FF f;
		if (x <= 0.5) return 0.5 * f(2.0*x);
		return 1 - 0.5 * f(2.0 - 2.0*x);
	}
};
}
void set_color(uc arr[3], COLOR c)
{
	arr[0] = (arr[0] * c.a + c.b * (255 - c.a) + 127) / 255;
	arr[1] = (arr[1] * c.a + c.g * (255 - c.a) + 127) / 255;
	arr[2] = (arr[2] * c.a + c.r * (255 - c.a) + 127) / 255;
}
void clear_all(COLOR bgc = COLOR(0u, 0u, 0u))
{
	for (uint i = 0;i < Height;++i)
		for (uint j = 0;j < Width;++j)
			set_color(BitMap[i][j], bgc);
}
struct PIC
{
	uint height, width;
	uint size;
	COLOR *pic_file;
	PIC (uint h, uint w):height(h), width(w), size(h * w){pic_file = new COLOR[size];}
	PIC (uint h, uint w, COLOR c):height(h), width(w), size(h * w)
	{
		pic_file = new COLOR[h * w];
		fill(pic_file, pic_file + h * w, c);
	}
	void draw(int bl, int bg)
	{
		for (uint i = max(-bl, 0);(int)i < min((int)Height - bl, (int)height);++i)
		{
			for (uint j = max(-bg, 0);(int)j < min((int)Width - bg, (int)width);++j)
			{
				set_color(BitMap[i + bl][j + bg], pic_file[i * width + j]);
			}
		}
	}
	void turn_color(COLOR prev, COLOR cur)
	{
		for (uint i = 0;i < size;++i)
		{
			if (pic_file[i] == prev)
			{
				pic_file[i] = cur;
			}
		}
	}
	void set_transparent_color(COLOR c)
	{
		turn_color(c, COLOR(0, 0, 0, 255));
	}
	template <typename FUNC>
	void show(uint Duration, uint Current_frame)
	{
		--Duration;
		FUNC f;
		uint alpha = (1.0 - f((double)Current_frame / Duration)) * 255 + 0.5;
		for (uint i = 0;i < size;++i)
		{
			pic_file[i].a = alpha;
		}
	}
};

struct FIGURE
{
	vector<PIXEL> Pixels;
	void draw(int bl, int br, COLOR c, uint num_visible, int th = 1)
	{
		int half_th = (th >> 1);
		for (uint i = 0;i < num_visible;++i)
		{
			for (int j = -half_th;j < th - half_th;++j)
				for (int k = -half_th;k < th - half_th;++k)
				{
					int l = bl + Pixels[i].line_num + j;
					int r = br + Pixels[i].row_num + k;
					if (l >= 0 && l < Height && r >= 0 && r < Width)
					set_color(BitMap[l][r], c);
				}
		}
	}
	template <typename FUNC>
	uint Show_Creation(uint Duration, uint Current_frame)
	{
		FUNC f;
		return f((double)Current_frame / (Duration - 1)) * Pixels.size() + 0.5;
	}
};

struct PARAEQ
{
	TRANS trans;
	double (*X)(double);
	double (*Y)(double);
	PARAEQ (TRANS trans, double (*X)(double), double (*Y)(double)):trans(trans), X(X), Y(Y){}
	void draw(FIGURE &fig, double begin, double end, double xmin, double xmax, double ymin, double ymax, double eps1 = 1e-3, double eps2 = 1e-6)
	{
		double mid = (begin + end) * 0.5;
		if (end - begin > eps1)
		{
			draw(fig, begin, mid, xmin, xmax, ymin, ymax, eps1, eps2);
			draw(fig, mid, end, xmin, xmax, ymin, ymax, eps1, eps2);
			return ;
		}
		COOR P(X(begin), Y(begin)), Q(X(end), Y(end));
		if (P.x < xmin || P.x > xmax || P.x != P.x || P.y < ymin || P.y > ymax || P.y != P.y)
			if (Q.x < xmin || Q.x > xmax || Q.x != Q.x || Q.y < ymin || Q.y > ymax || Q.y != Q.y)
				return ;
			else
			{
				draw(fig, mid, end, xmin, xmax, ymin, ymax, eps1, eps2);
				return ;
			}
		else if (Q.x < xmin || Q.x > xmax || Q.x != Q.x || Q.y < ymin || Q.y > ymax || Q.y != Q.y)
		{
			draw(fig, begin, mid, xmin, xmax, ymin, ymax, eps1, eps2);
			return ;
		}
		PIXEL B = trans.C2P(P), E = trans.C2P(Q);
		if (end - begin < eps2)
		{
			fig.Pixels.push_back(B);
			if (E.line_num != B.line_num || E.row_num != B.row_num)
				fig.Pixels.push_back(E);
		}
		int dx = B.line_num - E.line_num, dy = B.row_num - E.row_num;
		switch (dx * dx + dy * dy)
		{
			case 0:fig.Pixels.push_back(B);break;
			case 1:
			case 2:fig.Pixels.push_back(B);fig.Pixels.push_back(E);break;
			default:draw(fig, begin, mid, xmin, xmax, ymin, ymax, eps1, eps2);draw(fig, mid, end, xmin, xmax, ymin, ymax, eps1, eps2);
		}
 	}
};
double X(double t)
{
	return t;
}
double Y(double t)
{
	return t * t;
}

template <typename FUNC>
COLOR color_transform(COLOR begin, COLOR end, uint Duration, uint Current_frame)
{
	--Duration;
	COLOR ans;
	FUNC f;
	ans.r = begin.r + (end.r - begin.r) * f((double)Current_frame / Duration) + 0.5;
	ans.g = begin.g + (end.g - begin.g) * f((double)Current_frame / Duration) + 0.5;
	ans.b = begin.b + (end.r - begin.b) * f((double)Current_frame / Duration) + 0.5;
	return ans;
}
void to_file(const char *filename)
{
	uint size = write_bmp(BMPFILE, Height, Width, 24, &BitMap[0][0][0]);
	output_bmp(filename, BMPFILE, size);
}
void get_normal_transform_sequence(vector<PIXEL> *res, FIGURE &fig1, FIGURE &fig2)
{
	if (fig1.Pixels.size() == 1)
	{
		res[0] = fig2.Pixels;
		return ;
	}
	else if (fig2.Pixels.size() == 1)
	{
		for (uint i = 0;i < fig1.Pixels.size();++i)
		{
			res[i] = fig2.Pixels;
		}
		return ;
	}
	ull temp11 = 0ull, temp12 = 0ull, temp21 = 0ull, temp22 = 0ull;
	for (uint i = 0;i < fig1.Pixels.size();++i)
	{
		temp11 += fig1.Pixels[i].line_num;
		temp12 += fig1.Pixels[i].row_num;
	}
	temp11 = (temp11 + fig1.Pixels.size() / 2) / fig1.Pixels.size();
	temp12 = (temp12 + fig1.Pixels.size() / 2) / fig1.Pixels.size();
	for (uint i = 0;i < fig2.Pixels.size();++i)
	{
		temp21 += fig2.Pixels[i].line_num;
		temp22 += fig2.Pixels[i].row_num;
	}
	temp21 = (temp21 + fig2.Pixels.size() / 2) / fig2.Pixels.size();
	temp22 = (temp22 + fig2.Pixels.size() / 2) / fig2.Pixels.size();
	PIXEL C1(temp11, temp12), C2(temp21, temp22);
	if (C1 == C2) C2 = PIXEL(C1.line_num, C1.row_num + 1);
	FIGURE fig11, fig12, fig13, fig21, fig22, fig23;
	for (uint i = 0;i < fig1.Pixels.size();++i)
	{
		if (fig1.Pixels[i].line_num*C1.row_num < fig1.Pixels[i].row_num*C1.line_num)
		{
			fig11.Pixels.push_back(fig1.Pixels[i]);
		}
		else if (fig1.Pixels[i].line_num*C1.row_num > fig1.Pixels[i].row_num*C1.line_num)
		{
			fig12.Pixels.push_back(fig1.Pixels[i]);
		}
		else
		{
			fig13.Pixels.push_back(fig1.Pixels[i]);
		}
	}
	uint a, b, c, c1;
	a = fig11.Pixels.size();
	b = fig12.Pixels.size();
	c = fig13.Pixels.size();
	if (a > b)
	{
		if (a - b >= c)
		{
			c1 = 0;
		}
		else
		{
			c1 = (b+c-a)/2;
		}
	}
	else
	{
		if (b - a >= c)
		{
			c1 = c;
		}
		else
		{
			c1 = (b+c-a)/2;
		}
	}
	for (uint i = 0;i < c1;++i)
	{
		fig11.Pixels.push_back(fig13.Pixels[i]);
	}
	for (uint i = c1;i < c;++i)
	{
		fig12.Pixels.push_back(fig13.Pixels[i]);
	}
	if (fig11.Pixels.size() == 0)
	{
		fig11.Pixels.push_back(fig12.Pixels[fig12.Pixels.size()-1]);
		fig12.Pixels.pop_back();
	}
	if (fig12.Pixels.size() == 0)
	{
		fig12.Pixels.push_back(fig11.Pixels[fig11.Pixels.size()-1]);
		fig11.Pixels.pop_back();
	}
	for (uint i = 0;i < fig2.Pixels.size();++i)
	{
		if (fig2.Pixels[i].line_num*C2.row_num < fig2.Pixels[i].row_num*C2.line_num)
		{
			fig21.Pixels.push_back(fig2.Pixels[i]);
		}
		else if (fig2.Pixels[i].line_num*C2.row_num > fig2.Pixels[i].row_num*C2.line_num)
		{
			fig22.Pixels.push_back(fig2.Pixels[i]);
		}
		else
		{
			fig23.Pixels.push_back(fig2.Pixels[i]);
		}
	}
	a = fig21.Pixels.size();
	b = fig22.Pixels.size();
	c = fig23.Pixels.size();
	if (a > b)
	{
		if (a - b >= c)
		{
			c1 = 0;
		}
		else
		{
			c1 = (b+c-a)/2;
		}
	}
	else
	{
		if (b - a >= c)
		{
			c1 = c;
		}
		else
		{
			c1 = (b+c-a)/2;
		}
	}
	for (uint i = 0;i < c1;++i)
	{
		fig21.Pixels.push_back(fig23.Pixels[i]);
	}
	for (uint i = c1;i < c;++i)
	{
		fig22.Pixels.push_back(fig23.Pixels[i]);
	}
	if (fig21.Pixels.size() == 0)
	{
		fig21.Pixels.push_back(fig22.Pixels[fig22.Pixels.size()-1]);
		fig22.Pixels.pop_back();
	}
	if (fig22.Pixels.size() == 0)
	{
		fig22.Pixels.push_back(fig21.Pixels[fig21.Pixels.size()-1]);
		fig21.Pixels.pop_back();
	}
	get_normal_transform_sequence(res, fig11, fig21);
	get_normal_transform_sequence(res + fig11.Pixels.size(), fig12, fig22);
}
template<typename FUNC>
void fig_transform(FIGURE &ans, FIGURE &fig1 , uint Duration, uint Current_frame, vector<PIXEL> *tr)
{
	ans.Pixels.clear();
	--Duration;
	FUNC f;
	double nx = f((double)Current_frame / Duration);
	for (uint i = 0;i < fig1.Pixels.size();++i)
		for (uint j = 0;j < tr[i].size();++j)
		{
			PIXEL cur;
			cur.line_num = fig1.Pixels[i].line_num + nx * (tr[i][j].line_num - fig1.Pixels[i].line_num) + 0.5;
			cur.row_num = fig1.Pixels[i].row_num + nx * (tr[i][j].row_num - fig1.Pixels[i].row_num) + 0.5;
			ans.Pixels.push_back(cur);
		}
}
namespace CONSTANT
{
const COLOR RED = COLOR(255, 0, 0);
const COLOR GREEN = COLOR(0, 255, 0);
const COLOR BLUE = COLOR(0, 0, 255);
const COLOR BLACK = COLOR(0, 0, 0);
const COLOR WHITE = COLOR(255, 255, 255);
const COLOR YELLOW = COLOR(255, 255, 0);
const COLOR PURPLE = COLOR(255, 0, 255);
const COLOR CYAN = COLOR(0, 255, 255);
const COLOR TRANSPARENT = COLOR(0, 0, 0, 255);
const TRANS NORM_TRANS = TRANS(1.0, 0.0, 1.0, 0.0);
}
const double neg_inf = -1.0 / 0.0;
const double pos_inf = 1.0 / 0.0;
using namespace CONSTANT;
int main()
{
	FIGURE fig1, fig2;
	for (uint i = 400;i < 600u;++i)
	{
		for (uint j = 800;j < 1200u;++j)
		{
			fig1.Pixels.push_back(PIXEL(i, j));
		}
	}
	for (uint i = 500;i < 600u;++i)
	{
		for (uint j = 1000;j < 1200u;++j)
		{
			fig2.Pixels.push_back(PIXEL(i, j));
		}
	}
	FIGURE ans;
	vector<PIXEL> tr[fig1.Pixels.size()];
	get_normal_transform_sequence(tr, fig1, fig2);
	for (uint i = 0;i < 120u;++i)
	{
		fig_transform<FUNCTION::id>(ans, fig1, 120u, i, tr);
		ans.draw(0, 0, GREEN, ans.Pixels.size(), 1);
		char temp[64];
		sprintf(temp, "transform%d.bmp", i+1);
		to_file(temp);
		ans.draw(0, 0, BLACK, ans.Pixels.size(), 1);
	}
	return 0;
}