#pragma once
#include <type_traits>
#include <stdexcept>
#include <utility>

namespace cov {
	struct in_place_t {
		explicit in_place_t() = default;
	};
	constexpr in_place_t in_place;

	class invalidate_value final : public std::runtime_error {
	public:
		using std::runtime_error::runtime_error;
	};

	struct nullopt_t {
		explicit nullopt_t() = default;
	};
	constexpr nullopt_t nullopt;

	template<typename T>class optional final {
		bool m_available = false;
		typename std::aligned_storage<sizeof(T), alignof(T)>::type m_data;
		inline T* pointer()
		{
			if (!m_available)
				throw invalidate_value("Optional is unavailable.");
			return reinterpret_cast<T*>(&m_data);
		}
		inline T* const pointer() const
		{
			if (!m_available)
				throw invalidate_value("Optional is unavailable.");
			return reinterpret_cast<const T*>(&m_data);
		}
	public:
		constexpr bool has_value() const
		{
			return m_available;
		}
		constexpr T& value()
		{
			return *pointer();
		}
		constexpr const T& value() const
		{
			return *pointer();
		}
		template<typename U>
		constexpr T value_or(U&& val)
		{
			if (m_available)
				return value();
			else
				return val;
		}
		void swap(optional &op) noexcept
		{
			std::swap(m_available, op.m_available);
			std::swap(m_data, op.m_data);
		}
		template<typename...argsT>
		void emplace(argsT&&...args)
		{
			if (m_available)
				reset();
			m_available = true;
			::new (pointer()) T(std::forward<argsT>(args)...);
		}
		void reset()
		{
			if (m_available) {
				pointer()->T::~T();
				m_available = false;
			}
		}
		constexpr optional() noexcept = default;
		constexpr optional(nullopt_t) noexcept {}
		optional(const optional &op)
		{
			emplace(op.value());
		}
		optional(optional &&op) noexcept
		{
			std::swap(m_available, op.m_available);
			std::swap(m_data, op.m_data);
		}
		template<typename...argsT>
		constexpr optional(in_place_t, argsT&&...args)
		{
			emplace(std::forward<argsT>(args)...);
		}
		~optional()
		{
			reset();
		}
		optional& operator=(nullopt_t) noexcept
		{
			reset();
		}
		optional& operator=(const optional& op)
		{
			if (&op != this) {
				reset();
				if (op.has_value())
					emplace(op.value());
			}
			return *this;
		}
		optional& operator=(optional &&op) noexcept
		{
			std::swap(m_available, op.m_available);
			std::swap(m_data, op.m_data);
		}
		explicit operator bool() const
		{
			return has_value();
		}
		T& operator*()
		{
			return value();
		}
		T* operator->()
		{
			return &value();
		}
	};
}