#include <iostream>
#include "any.hpp"

int main()
{
	std::any a = std::make_any<int>(10);
	++std::any_cast<int>(a);
	std::cout << std::any_cast<int>(a) << std::endl;
	a = std::string("Hello");
	std::cout << std::any_cast<std::string>(a) << std::endl;
	std::any b = a;
	std::cout << std::any_cast<std::string>(b) << std::endl;
	return 0;
}
