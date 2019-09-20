#define COVSCRIPT_SDK_DEBUG

#include "any.hpp"
#include <iostream>
#include <string>
#include <vector>

int main()
{
	std::cout << "Size of any: " << sizeof(cov::any) << std::endl;
	std::cout << "Size of size_t: " << sizeof(std::size_t) << std::endl;
	std::cout << "Size of string: " << sizeof(std::string) << std::endl;
	std::cout << "Size of vector: " << sizeof(std::vector<cov::any>) << std::endl;
	cov::any a, b(10), c(std::string("Hello"));
	cov::any d(a), e(b), f(c);
	a = 3.14;
	d = a;
	std::cout << a.get<double>() << std::endl;
	std::cout << b.get<int>() << std::endl;
	std::cout << c.get<std::string>() << std::endl;
	std::cout << d.get<double>() << std::endl;
	std::cout << e.get<int>() << std::endl;
	std::cout << f.get<std::string>() << std::endl;
	return 0;
}