#define COVSCRIPT_SDK_DEBUG
#define COVSDK_LOGCR_ONLY

#include "any.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <any>

class timer final {
	static std::chrono::time_point<std::chrono::high_resolution_clock> m_timer;

public:
	typedef std::size_t timer_t;
	enum class time_unit {
		nano_sec,
		micro_sec,
		milli_sec,
		second,
		minute
	};

	static void reset()
	{
		m_timer = std::chrono::high_resolution_clock::now();
	}

	static timer_t time(time_unit unit = time_unit::milli_sec)
	{
		switch (unit) {
		case time_unit::nano_sec:
			return std::chrono::duration_cast<std::chrono::nanoseconds>(
			           std::chrono::high_resolution_clock::now() - m_timer)
			       .count();
		case time_unit::micro_sec:
			return std::chrono::duration_cast<std::chrono::microseconds>(
			           std::chrono::high_resolution_clock::now() - m_timer)
			       .count();
		case time_unit::milli_sec:
			return std::chrono::duration_cast<std::chrono::milliseconds>(
			           std::chrono::high_resolution_clock::now() - m_timer)
			       .count();
		case time_unit::second:
			return std::chrono::duration_cast<std::chrono::seconds>(
			           std::chrono::high_resolution_clock::now() - m_timer)
			       .count();
		case time_unit::minute:
			return std::chrono::duration_cast<std::chrono::minutes>(
			           std::chrono::high_resolution_clock::now() - m_timer)
			       .count();
		}
		return 0;
	}

	static timer_t measure(const std::function<void()> &func, time_unit unit = time_unit::milli_sec)
	{
		timer_t begin(0), end(0);
		begin = time(unit);
		func();
		end = time(unit);
		return end - begin;
	}
};

std::chrono::time_point<std::chrono::high_resolution_clock>
timer::m_timer(std::chrono::high_resolution_clock::now());

int test_epoch = 10000000;

struct test_large_data {
	std::string str;
	unsigned char alignment[32];
	test_large_data(const char *s) : str(s) {}
};

int main()
{
	std::cout << "Size of std::any: " << sizeof(std::any) << std::endl;
	std::cout << "Size of cov::any: " << sizeof(cov::any) << std::endl;

	std::cout << "[Small Data] std::any copying: " << timer::measure([]() {
		std::any a(10);
		for (int i = 0; i < test_epoch; ++i)
			std::any b(a);
	}) << std::endl;
	std::cout << "[Small Data] cov::any copying: " << timer::measure([]() {
		cov::any a(10);
		for (int i = 0; i < test_epoch; ++i)
			cov::any b(a);
	}) << std::endl;

	std::cout << "[Small Data] std::any instancing: " << timer::measure([]() {
		std::any a(10);
		for (int i = 0; i < test_epoch; ++i) {
			++std::any_cast<int &>(a);
			--std::any_cast<int &>(a);
		}
	}) << std::endl;
	std::cout << "[Small Data] cov::any instancing: " << timer::measure([]() {
		cov::any b(10);
		for (int i = 0; i < test_epoch; ++i) {
			++b.get<int>();
			--b.get<int>();
		}
	}) << std::endl;

	std::cout << "[Large Data] std::any copying: " << timer::measure([]() {
		std::any a(test_large_data("Hello"));
		for (int i = 0; i < test_epoch; ++i)
			std::any b(a);
	}) << std::endl;
	std::cout << "[Large Data] cov::any copying: " << timer::measure([]() {
		cov::any a(test_large_data("Hello"));
		for (int i = 0; i < test_epoch; ++i)
			cov::any b(a);
	}) << std::endl;

	std::cout << "[Large Data] std::any instancing: " << timer::measure([]() {
		std::any a(test_large_data("Hello"));
		for (int i = 0; i < test_epoch; ++i) {
			std::any_cast<test_large_data &>(a).str.push_back('c');
			std::any_cast<test_large_data &>(a).str.pop_back();
		}
	}) << std::endl;
	std::cout << "[Large Data] cov::any instancing: " << timer::measure([]() {
		cov::any b(test_large_data("Hello"));
		for (int i = 0; i < test_epoch; ++i) {
			b.get<test_large_data>().str.push_back('c');
			b.get<test_large_data>().str.pop_back();
		}
	}) << std::endl;

	return 0;
}
