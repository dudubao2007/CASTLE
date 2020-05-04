#ifndef TIME_STAMP_HPP
#define TIME_STAMP_HPP

#include <chrono>
#include <iostream>
#include <string>
namespace CASTLE {
//计时器
//构造时启动，析构时停止并输出
struct TimeStamp {
	std::chrono::high_resolution_clock::time_point t;
	std::string s;
	TimeStamp(std::string s = "")
		: s(s)
		, t(std::chrono::high_resolution_clock::now()) {}
	~TimeStamp() {
		std::cout << s << (s.size() ? " " : "") << "Time Used:"
				  << (std::chrono::high_resolution_clock::now() - t).count()
				/ 1e6
				  << "ms" << std::endl;
	}
};
} // namespace CASTLE
#endif // TIME_STAMP_HPP
