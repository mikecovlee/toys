#include <iostream>
#include <string>
#include "./variant.hpp"

using variant_impl::variant;

int main()
{
	variant<int, double, std::string> var;
	std::cout<<var.type().name()<<std::endl;
	var=10;
	std::cout<<var.get<int>()<<std::endl;
	var=3.14;
	std::cout<<static_cast<double>(var)<<std::endl;
	var=std::string("Hello");
	std::cout<<var.get<std::string>()<<std::endl;
	return 0;
}