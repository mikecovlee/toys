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
    cov::vector<int> cov_vec;
    auto ts = 0;
	ts = time();
    for (long i = 0; i < test_length; ++i) {
		cov_vec.push_back(cov::rand(long(1), test_range));
	}
	std::cout << "COV_best:" << time() - ts << std::endl;
}