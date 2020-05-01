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
	auto f = [width, get_color, &bitmap](Dword s, Dword t) {
		for (Dword i = s; i < t; ++i)
			for (Dword j = 0; j < width; ++j) {
				auto t = get_color(i, j);
				bitmap.data[i][j][0] = get<0>(t);
				bitmap.data[i][j][1] = get<1>(t);
				bitmap.data[i][j][2] = get<2>(t);
			}
	};
	Dword p = height / thread_num;
	for (Dword i = 0; i < thread_num; ++i)
		thd_pool[i] = thread(f, i * p, i * p + p);
	for (Dword i = 0; i < thread_num; ++i)
		thd_pool[i].join();
	f(thread_num * p, height);
}
} // namespace Render
