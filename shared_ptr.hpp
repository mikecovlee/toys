#pragma once
#include <cstddef>
#include <utility>

namespace cov {
	template <typename T>
	class shared_ptr final {
		struct proxy {
			std::size_t ref_count = 0;
			T *data_ptr = nullptr;
			proxy(T *ptr) : data_ptr(ptr) {}
			~proxy()
			{
				delete data_ptr;
			}
		};
		proxy *m_ptr = nullptr;
		inline proxy *copy()
		{
			if (m_ptr != nullptr)
				++(m_ptr->ref_count);
			return m_ptr;
		}
		inline void recycle()
		{
			if (m_ptr != nullptr && --(m_ptr->ref_count) == 0)
				delete m_ptr;
		}

	public:
		constexpr shared_ptr() noexcept = default;
		explicit shared_ptr(T *ptr) : m_ptr(new proxy(ptr)) {}
		shared_ptr(const shared_ptr &sp) noexcept : m_ptr(sp.copy()) {}
		shared_ptr(shared_ptr &&sp) noexcept
		{
			std::swap(m_ptr, sp.m_ptr);
		}
		~shared_ptr()
		{
			recycle();
		}
		inline shared_ptr &operator=(const shared_ptr &sp)
		{
			if (&sp != this) {
				recycle();
				m_ptr = sp.copy();
			}
			return *this;
		}
		inline shared_ptr &operator=(shared_ptr &&sp) noexcept
		{
			std::swap(m_ptr, sp.m_ptr);
			return *this;
		}
		inline void reset()
		{
			recycle();
		}
		inline void reset(T *ptr)
		{
			recycle();
			m_ptr = new proxy(ptr);
		}
		inline void swap(shared_ptr &sp) noexcept
		{
			std::swap(m_ptr, sp.m_ptr);
		}
		inline T *get() const noexcept
		{
			if (m_ptr != nullptr)
				return m_ptr->data_ptr;
			else
				return nullptr;
		}
		inline std::size_t use_count() const noexcept
		{
			if (m_ptr != nullptr)
				return m_ptr->ref_count;
			else
				return 0;
		}
		inline T *operator->() const noexcept
		{
			return get();
		}
		inline T &operator*() const noexcept
		{
			return *get();
		}
		inline operator bool() const noexcept
		{
			return m_ptr != nullptr;
		}
	};
	template <typename T, typename... ArgsT>
	shared_ptr<T> make_shared(ArgsT &&... args)
	{
		return shared_ptr<T>(new T(std::forward<ArgsT>(args)...));
	}
} // namespace cov