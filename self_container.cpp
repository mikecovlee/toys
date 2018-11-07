#pragma once
#include <functional>
#include <iostream>
#include <utility>
template <typename T>
class self_container;
template <typename return_type, typename... args_type>
class self_container<return_type(args_type...)> final {
	std::function<return_type(const self_container &, args_type...)> m_func;

public:
	self_container() = delete;
	template <typename T>
	explicit self_container(T &&func) : m_func(std::forward<T>(func)) {}
	self_container(const self_container &) = default;
	self_container(self_container &&) noexcept = default;
	template <typename... element_t>
	return_type operator()(element_t &&... args) const
	{
		return std::move(m_func(*this, std::forward<element_t>(args)...));
	}
};
int main()
{
	self_container<int(int)> fib([](const self_container<int(int)> &self, int n) {
		return n > 2 ? self(n - 1) + self(n - 2) : 1;
	});
	int n;
	while (std::cin >> n)
		std::cout << fib(n) << std::endl;
	return 0;
}