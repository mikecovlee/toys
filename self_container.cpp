#include <functional>
#include <iostream>
#include <utility>
template <typename T>
class self_container;
template <typename T>
struct mem_fn_analyzer;
template <typename T>
struct remove_self;
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
template <typename T, typename rT, typename... argsT>
struct mem_fn_analyzer<rT (T::*)(argsT...)> final {
	using plain_type = rT(argsT...);
};
template <typename T, typename rT, typename... argsT>
struct mem_fn_analyzer<rT (T::*)(argsT...) const> final {
	using plain_type = rT(argsT...);
};
template <typename rT, typename... argsT>
struct remove_self<rT(const self_container<rT(argsT...)> &, argsT...)> {
	using result = rT(argsT...);
};
template <typename T>
struct lambda_analyzer {
	using plain_type = typename remove_self<typename mem_fn_analyzer<decltype(&T::operator())>::plain_type>::result;
};
template <typename T>
auto make_lambda(T &&func) -> self_container<typename lambda_analyzer<T>::plain_type>
{
	return self_container<typename lambda_analyzer<T>::plain_type>(std::forward<T>(func));
}
#define compose_lambda(CAPTURE, RTYPE, ...) CAPTURE(const self_container<RTYPE(__VA_ARGS__)> &self, __VA_ARGS__)->RTYPE
int main()
{
	auto fib = make_lambda(compose_lambda([], int, int n) {
		return n > 2 ? self(n - 1) + self(n - 2) : 1;
	});
	int n;
	while (std::cin >> n)
		std::cout << fib(n) << std::endl;
	return 0;
}