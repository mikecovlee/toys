#include "optional.hpp"
#include <iostream>
#include <string>

void test(cov::optional<std::string> value)
{
	if (value.has_value())
		std::cout << *value << std::endl;
	else
		std::cout << "Invalidate" << std::endl;
}

int main()
{
	while (true) {
		std::string value;
		std::getline(std::cin, value);
		if (!value.empty() && value != "fuck")
			test({cov::in_place, value});
		else
			test(cov::nullopt);
	}
	return 0;
}