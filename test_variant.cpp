#include <iostream>
#include <string>
#include "./variant.hpp"

using variant_impl::variant;

int main()
{
    variant<int, double, std::string> var(10);
    std::cout<<var.get<int>()<<std::endl;
    var=std::string("Hello");
    std::cout<<var.get<std::string>()<<std::endl;
    return 0;
}