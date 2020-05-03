#ifndef RENDER_HPP
#define RENDER_HPP
#include "global_define.hpp"
#include "picture.hpp"
#include "shape.hpp"
#include <functional>
#include <thread>
#include <vector>
namespace CASTLE {
//一般渲染器
namespace Render {
	template <typename T>
	void render(BasePicture<T> &bitmap,
		std::function<T(Dword, Dword)> get_color, Dword thread_num = 1) {

		using namespace std;

		Dword height = bitmap.height();
		Dword width = bitmap.width();
		vector<thread> thd_pool(thread_num);

		// 渲染第 begin 行到第 end 行 (不含第 end 行)
		auto f = [width, get_color, &bitmap](Dword begin, Dword end) {
			for (Dword i = begin; i < end; ++i)
				for (Dword j = 0; j < width; ++j) {
					bitmap.data[i][j] = get_color(i, j);
				}
		};

		// 将图片按行分成 thread_num 组, 各线程负责一组
		Dword p = height / thread_num;
		for (Dword i = 0; i < thread_num; ++i)
			thd_pool[i] = thread(f, i * p, (i + 1) * p);
		for (Dword i = 0; i < thread_num; ++i)
			thd_pool[i].join();
		// 剩余的组在这里渲染
		f(thread_num * p, height);
	}
	void render_shape(ColorExtPicture &pic, const Shape::Shape &shape,
		std::function<ColorExt(Float)> sdf_dealer, Dword thread_num = 1) {
		std::function<ColorExt(Dword, Dword)> f
			= [&shape, sdf_dealer](Dword i, Dword j) {
				  return sdf_dealer(shape.sdf(Coordinate<Float>(j, i)));
			  };
		render(pic, f, thread_num);
	}
	void render_shape(ColorExtPicture &pic, const Shape::Shape &shape,
		ColorExt InColor, ColorExt EdgeColor, Float LineWidth,
		Dword thread_num = 1) {
		std::function<ColorExt(Float)> sdf_dealer
			= [InColor, EdgeColor, LineWidth](Float sdf) {
				  Float halfLW = 0.5 * LineWidth;
				  if (-halfLW < sdf && sdf < halfLW)
					  return EdgeColor;
				  if (sdf < 0)
					  return InColor;
				  return ColorExt(0.0, 0.0, 0.0, 1.0);
			  };
		render_shape(pic, shape, sdf_dealer, thread_num);
	}

} // namespace Render
} // namespace CASTLE
#endif // RENDER_HPP
