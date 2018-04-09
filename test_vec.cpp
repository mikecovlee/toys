#include <iostream>
#include <vector>
#include "./vector.hpp"
#include <mozart/timer.hpp>
#include <mozart/random.hpp>

cov::timer::timer_t time()
{
	return cov::timer::time(cov::timer::time_unit::milli_sec);
}

int main()
{
	long test_length = 10000000;
	long test_range = 10000;
	std::vector<int> std_vec;
	cov::vector<int, std::size_t, 100000> cov_vec;
	unsigned long ts = 0, sum = 0;
	ts = time();
	for (long i = 0; i < test_length; ++i) {
		cov_vec.push_back(cov::rand(long(1), test_range));
	}
	std::cout << "COV Push:" << time() - ts << std::endl;
	std::cout << cov_vec.capacity() << std::endl;
	ts = time();
	sum = 0;
	for (long i = 0; i < test_length; ++i) {
		sum += cov_vec[i];
	}
	std::cout << "COV Sum:" << time() - ts << std::endl;
	std::cout << sum << std::endl;
	ts = time();
	for (long i = 0; i < test_length; ++i) {
		cov_vec.pop_back();
	}
	std::cout << "COV Pop:" << time() - ts << std::endl;
	std::cout << cov_vec.capacity() << std::endl;
	ts = time();
	for (long i = 0; i < test_length; ++i) {
		std_vec.push_back(cov::rand(long(1), test_range));
	}
	std::cout << "STD Push:" << time() - ts << std::endl;
	std::cout << std_vec.capacity() << std::endl;
	ts = time();
	sum = 0;
	for (long i = 0; i < test_length; ++i) {
		sum += std_vec[i];
	}
	std::cout << "STD Sum:" << time() - ts << std::endl;
	std::cout << sum << std::endl;
	ts = time();
	for (long i = 0; i < test_length; ++i) {
		std_vec.pop_back();
	}
	std::cout << "STD Pop:" << time() - ts << std::endl;
	std::cout << std_vec.capacity() << std::endl;
	return 0;
}