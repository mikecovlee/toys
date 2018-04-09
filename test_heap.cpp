#include <iostream>
#include <memory>
#include "heap.hpp"
#include <mozart/timer.hpp>
#include <mozart/random.hpp>

cov::heap<10240, cov::allocate_policy::best_fit, true> cov_alloc_best;
cov::heap<10240, cov::allocate_policy::first_fit, true> cov_alloc_first;
cov::heap<10240, cov::allocate_policy::worst_fit, true> cov_alloc_worst;
std::allocator<cov::byte> std_alloc;

cov::timer::timer_t time()
{
	return cov::timer::time(cov::timer::time_unit::milli_sec);
}

int main()
{
	long test_length = 10000000;
	long test_range = 10000;
	auto ts = 0;
	ts = time();
	for (long i = 0; i < test_length; ++i) {
		void *ptr = cov_alloc_best.malloc(cov::rand(long(1), test_range));
		cov_alloc_best.free(ptr);
	}
	std::cout << "COV_best:" << time() - ts << std::endl;
	ts = time();
	for (long i = 0; i < test_length; ++i) {
		void *ptr = cov_alloc_first.malloc(cov::rand(long(1), test_range));
		cov_alloc_first.free(ptr);
	}
	std::cout << "COV_first:" << time() - ts << std::endl;
	ts = time();
	for (long i = 0; i < test_length; ++i) {
		void *ptr = cov_alloc_worst.malloc(cov::rand(long(1), test_range));
		cov_alloc_worst.free(ptr);
	}
	std::cout << "COV_worst:" << time() - ts << std::endl;
	ts = time();
	for (long i = 0; i < test_length; ++i) {
		long size = cov::rand(long(1), test_range);
		cov::byte *ptr = std_alloc.allocate(size);
		std_alloc.deallocate(ptr, size);
	}
	std::cout << "STD:" << time() - ts << std::endl;
	return 0;
}