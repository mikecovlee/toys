#include <iostream>
#include "any.hpp"

int main()
{
	std::any *a = new std::any(std::make_any<int>(10));
	++*std::any_cast<int>(a);
	std::cout << *std::any_cast<long>(a) << std::endl;
	*a = std::string("Hello");
	std::cout << *std::any_cast<std::string>(a) << std::endl;
	delete a;
	return 0;
}
