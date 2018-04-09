#pragma once
// Linear Container by Michael Lee
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
	class linear_container {
	protected:
		static allocator_t<data_t> m_allocator;
		size_t m_capacity = 0, m_size = 0;
		data_t *m_data = nullptr;
		virtual void extend()=0;
	public:
		constexpr linear_container() = default;

		linear_container(const linear_container &lc) : m_capacity(lc.m_capacity), m_size(lc.m_size),
			m_data(m_allocator.allocate(lc.m_capacity))
		{
			normal_copy_n(lc.m_data, m_data, m_size);
		}

		linear_container(linear_container &&lc) noexcept
		{
			swap(lc);
		}

		virtual ~linear_container()
		{
			clear();
		}

		linear_container &operator=(const linear_container &lc)
		{
			assign(lc);
			return *this;
		}

		linear_container &operator=(linear_container &&lc) noexcept
		{
			swap(lc);
			return *this;
		}

		void assign(const linear_container &lc)
		{
			if (&lc != this) {
				clear();
				m_capacity = lc.m_capacity;
				m_size = lc.m_size;
				m_data = m_allocator.allocate(m_capacity);
				normal_copy_n(lc.m_data, m_data, m_size);
			}
		}

		data_t &at(size_t idx)
		{
			if (idx >= m_size)
				throw std::out_of_range("linear container");
			else
				return m_data[idx];
		}

		const data_t &at(size_t idx) const
		{
			if (idx >= m_size)
				throw std::out_of_range("linear container");
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

		virtual void reserve(size_t)=0;

		size_t capacity() const
		{
			return m_capacity;
		}

		virtual void shrink_to_fit()=0;

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
				extend();
			::new(m_data + (m_size++)) data_t(std::forward<args_t>(args)...);
		}

		void push_back(const data_t &dat)
		{
			emplace_back(std::move(dat));
		}

		void pop_back()
		{
			if (m_size == 0)
				throw std::logic_error("Pop back from empty linear container.");
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

		void swap(linear_container &lc)
		{
			std::swap(lc.m_capacity, m_capacity);
			std::swap(lc.m_size, m_size);
			std::swap(lc.m_data, m_data);
		}

		void swap(linear_container &&lc) noexcept
		{
			std::swap(lc.m_capacity, m_capacity);
			std::swap(lc.m_size, m_size);
			std::swap(lc.m_data, m_data);
		}
	};

	template<typename data_t, typename size_t=std::size_t, size_t chunk_size = 32, template<typename> class allocator_t=std::allocator>
	class array_list final:public linear_container<data_t,size_t,allocator_t> {
		using parent_t=linear_container<data_t,size_t,allocator_t>;

		size_t compute_capacity(size_t cap)
		{
			if (cap % chunk_size == 0)
				return cap;
			else
				return ((cap - cap % chunk_size) / chunk_size + 1) * chunk_size;
		}

		virtual void extend() override
		{
			this->reserve(chunk_size);
		}

	public:
		using parent_t::parent_t;

		virtual void reserve(size_t new_cap) override
		{
			if (this->m_capacity - this->m_size < chunk_size) {
				size_t cap = this->m_capacity + compute_capacity(new_cap);
				data_t *dat = parent_t::m_allocator.allocate(cap);
				uninitialized_copy_n(this->m_data, dat, this->m_size);
				if (this->m_data != nullptr)
					parent_t::m_allocator.deallocate(this->m_data, this->m_capacity);
				this->m_capacity = cap;
				this->m_data = dat;
			}
		}

		virtual void shrink_to_fit() override
		{
			if (this->m_capacity - this->m_size > chunk_size) {
				size_t cap = compute_capacity(this->m_size);
				data_t *dat = parent_t::m_allocator.allocate(cap);
				uninitialized_copy_n(this->m_data, dat, this->m_size);
				if (this->m_data != nullptr)
					parent_t::m_allocator.deallocate(this->m_data, this->m_capacity);
				this->m_capacity = cap;
				this->m_data = dat;
			}
		}
	};

	template<typename data_t, typename size_t=std::size_t, template<typename> class allocator_t=std::allocator>
	class vector final:public linear_container<data_t,size_t,allocator_t> {
		using parent_t=linear_container<data_t,size_t,allocator_t>;
		virtual void extend() override
		{
			this->reserve(this->m_capacity>0?this->m_capacity:1);
		}
	public:
		using parent_t::parent_t;

		virtual void reserve(size_t new_cap) override
		{
			if (this->m_capacity - this->m_size < new_cap) {
				size_t cap = this->m_size + new_cap;
				data_t *dat = parent_t::m_allocator.allocate(cap);
				uninitialized_copy_n(this->m_data, dat, this->m_size);
				if (this->m_data != nullptr)
					parent_t::m_allocator.deallocate(this->m_data, this->m_capacity);
				this->m_capacity = cap;
				this->m_data = dat;
			}
		}

		virtual void shrink_to_fit() override
		{
			if (this->m_capacity > this->m_size) {
				size_t cap = this->m_size;
				data_t *dat = parent_t::m_allocator.allocate(cap);
				uninitialized_copy_n(this->m_data, dat, this->m_size);
				if (this->m_data != nullptr)
					parent_t::m_allocator.deallocate(this->m_data, this->m_capacity);
				this->m_capacity = cap;
				this->m_data = dat;
			}
		}
	};

	template<typename data_t, typename size_t, template<typename> class allocator_t> allocator_t<data_t> linear_container<data_t, size_t, allocator_t>::m_allocator;
}