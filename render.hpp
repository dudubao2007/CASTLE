#ifndef RENDER_HPP
#define RENDER_HPP

#include "global_define.hpp"
#include "picture.hpp"
#include <cassert>
#include <functional>
#include <thread>
#include <tuple>
#include <vector>

namespace Render {

void render(BytePicture bitmap,
	std::function<std::tuple<Byte, Byte, Byte>(Dword, Dword)> get_color,
	Dword thread_num = 1) {

	using namespace std;

	Dword height = bitmap.height();
	Dword width = bitmap.width();
	vector<thread> thd_pool(thread_num);

	// 渲染第 begin 行到第 end 行 (不含第 end 行)
	auto f = [width, get_color, &bitmap](Dword begin, Dword end) {
		for (Dword i = begin; i < end; ++i)
			for (Dword j = 0; j < width; ++j) {
				auto c = get_color(i, j);
				bitmap.data[i][j][0] = get<0>(c);
				bitmap.data[i][j][1] = get<1>(c);
				bitmap.data[i][j][2] = get<2>(c);
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

} // namespace Render

#endif // RENDER_HPP
