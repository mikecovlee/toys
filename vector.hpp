#pragma once
// std::vector by Michael Lee
#include <stdexcept>
#include <memory>

namespace cov {
	template<typename data_t, typename size_t>
	void uninitialized_copy_n(data_t *src, data_t *dest, size_t count)
	{
		if (count > 0) {
			using byte_t=unsigned char;
			byte_t *_src = reinterpret_cast<byte_t *>(src);
			byte_t *_dest = reinterpret_cast<byte_t *>(dest);
			size_t _count = count * sizeof(data_t);
			for (size_t idx = 0; idx < _count; ++idx)
				*(_dest + idx) = *(_src + idx);
		}
	}

	template<typename data_t, typename size_t>
	void normal_copy_n(data_t *src, data_t *dest, size_t count)
	{
		if (count > 0) {
			for (size_t idx = 0; idx < count; ++idx)
				::new(dest + idx) data_t(*(src + idx));
		}
	}

	template<typename data_t, typename size_t=std::size_t, template<typename> class allocator_t=std::allocator>
	class vector final {
		static allocator_t<data_t> m_allocator;
		size_t m_capacity = 0, m_size = 0;
		data_t *m_data = nullptr;

	public:
		constexpr vector() = default;

		vector(const vector &vec) : m_capacity(vec.m_capacity), m_size(vec.m_size),
			m_data(m_allocator.allocate(vec.m_capacity))
		{
			normal_copy_n(vec.m_data, m_data, m_size);
		}

		vector(vector &&vec) noexcept
		{
			swap(vec);
		}

		~vector()
		{
			clear();
		}

		vector &operator=(const vector &vec)
		{
			assign(vec);
			return *this;
		}

		vector &operator=(vector &&vec) noexcept
		{
			swap(vec);
			return *this;
		}

		void assign(const vector &vec)
		{
			if (&vec != this) {
				clear();
				m_capacity = vec.m_capacity;
				m_size = vec.m_size;
				m_data = m_allocator.allocate(m_capacity);
				normal_copy_n(vec.m_data, m_data, m_size);
			}
		}

		data_t &at(size_t idx)
		{
			if (idx >= m_size)
				throw std::out_of_range("Vector");
			else
				return m_data[idx];
		}

		const data_t &at(size_t idx) const
		{
			if (idx >= m_size)
				throw std::out_of_range("Vector");
			else
				return m_data[idx];
		}

		data_t &operator[](size_t idx)
		{
			return m_data[idx];
		}

		const data_t &operator[](size_t idx) const
		{
			return m_data[idx];
		}

		data_t &front()
		{
			return *m_data;
		}

		const data_t &front() const
		{
			return *m_data;
		}

		data_t &back()
		{
			return *(m_data + m_size - 1);
		}

		const data_t &back() const
		{
			return *(m_data + m_size - 1);
		}

		data_t *data() const
		{
			return m_data;
		}

		bool empty() const
		{
			return m_size == 0;
		}

		size_t size() const
		{
			return m_size;
		}

		void reserve(size_t new_cap)
		{
			if (m_capacity - m_size < new_cap) {
				size_t cap = m_size + new_cap;
				data_t *dat = m_allocator.allocate(cap);
				uninitialized_copy_n(m_data, dat, m_size);
				if (m_data != nullptr)
					m_allocator.deallocate(m_data, m_capacity);
				m_capacity = cap;
				m_data = dat;
			}
		}

		size_t capacity() const
		{
			return m_capacity;
		}

		void shrink_to_fit()
		{
			if (m_capacity > m_size) {
				size_t cap = m_size;
				data_t *dat = m_allocator.allocate(cap);
				uninitialized_copy_n(m_data, dat, m_size);
				if (m_data != nullptr)
					m_allocator.deallocate(m_data, m_capacity);
				m_capacity = cap;
				m_data = dat;
			}
		}

		void clear()
		{
			for (size_t idx = 0; idx < m_size; ++idx)
				(m_data + idx)->~data_t();
			if (m_data != nullptr)
				m_allocator.deallocate(m_data, m_capacity);
			m_capacity = 0;
			m_size = 0;
			m_data = nullptr;
		}

		template<typename...args_t>
		void emplace_back(args_t &&...args)
		{
			if (m_size == m_capacity)
				reserve(m_capacity>0?m_capacity:1);
			::new(m_data + (m_size++)) data_t(std::forward<args_t>(args)...);
		}

		void push_back(const data_t &dat)
		{
			emplace_back(std::move(dat));
		}

		void pop_back()
		{
			if (m_size == 0)
				throw std::logic_error("Pop back from empty vector.");
			else
				(m_data + (m_size--))->~data_t();
		}

		void resize(size_t count, const data_t &dat)
		{
			if (count == m_size)
				return;
			if (count > m_size) {
				reserve(count);
				while (m_size < count)
					::new(m_data + (m_size++)) data_t(dat);
			}
			else {
				while (m_size > count)
					(m_data + (m_size--))->~data_t();
			}
		}

		void resize(size_t count)
		{
			resize(count, data_t());
		}

		void swap(vector &vec)
		{
			std::swap(vec.m_capacity, m_capacity);
			std::swap(vec.m_size, m_size);
			std::swap(vec.m_data, m_data);
		}

		void swap(vector &&vec) noexcept
		{
			std::swap(vec.m_capacity, m_capacity);
			std::swap(vec.m_size, m_size);
			std::swap(vec.m_data, m_data);
		}
	};

	template<typename data_t, typename size_t, template<typename> class allocator_t> allocator_t<data_t> vector<data_t, size_t, allocator_t>::m_allocator;
}