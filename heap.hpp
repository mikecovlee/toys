#pragma once

#include <stdexcept>
#include <cstdint>
#include <utility>
#include <list>

namespace cov {
	using byte=uint8_t;
	using size_t=uint64_t;
	enum class allocate_policy {
		first_fit, best_fit, worst_fit
	};

	template<size_t pool_size, allocate_policy policy = allocate_policy::best_fit, bool no_truncate = false>
	class heap final {
		// Heap Start
		byte hs[pool_size];
		// Heap Pointer
		byte *hp = nullptr;
		// Heap Limit
		byte *hl = nullptr;
		std::list<byte *> free_list;

		inline size_t &get_size(byte *ptr)
		{
			return *reinterpret_cast<size_t *>(ptr);
		}

		void compress()
		{
			std::list<byte *> new_list;
			byte *ptr = nullptr;
			// Sort the spaces by address.
			free_list.sort([this](byte *lhs, byte *rhs) {
				return lhs < rhs;
			});
			// Compress the free list.
			for (auto p:free_list) {
				if (ptr != nullptr) {
					size_t &size = get_size(ptr);
					if (ptr + size + sizeof(size_t) == p) {
						size += get_size(p) + sizeof(size_t);
					}
					else {
						new_list.push_back(ptr);
						ptr = p;
					}
				}
				else
					ptr = p;
			}
			// Connect the final space and remain spaces.
			if (ptr != nullptr) {
				if (ptr + get_size(ptr) + sizeof(size_t) == hp)
					hp = ptr;
				else
					new_list.push_back(ptr);
			}
			// Swap the new list and old list.
			std::swap(new_list, free_list);
		}

		byte *allocate(size_t size)
		{
			// Try to find usable spaces in free list
			if (!free_list.empty()) {
				auto it = free_list.begin();
				switch (policy) {
				case allocate_policy::first_fit: {
					// Find the first fit space.
					for (; it != free_list.end(); ++it)
						if (get_size(*it) >= size)
							break;
					break;
				}
				case allocate_policy::best_fit: {
					// Find the best fit space.
					auto best = it;
					for (; it != free_list.end(); ++it)
						if (get_size(*it) >= size && get_size(*it) < get_size(*best))
							best = it;
					it = best;
					break;
				}
				case allocate_policy::worst_fit: {
					// Find the worst fit space.
					auto max = it;
					for (; it != free_list.end(); ++it)
						if (get_size(*it) > get_size(*max))
							max = it;
					it = max;
					break;
				}
				}
				if (it != free_list.end() && get_size(*it) >= size) {
					// Remove from free list.
					byte *raw = *it;
					free_list.erase(it);
					// Truncate remain spaces
					if (!no_truncate && get_size(raw) - size > sizeof(size_t)) {
						byte *ptr = raw + sizeof(size_t) + size;
						get_size(ptr) = get_size(raw) - size - sizeof(size_t);
						get_size(raw) = size;
						free_list.push_back(ptr);
					}
					return raw + sizeof(size_t);
				}
			}
			// Checkout remain spaces,if enough,return.
			if (hl - hp >= size + sizeof(size_t)) {
				get_size(hp) = size;
				byte *ptr = hp + sizeof(size_t);
				hp += size + sizeof(size_t);
				return ptr;
			}
			return nullptr;
		}

	public:
		heap()
		{
			hp = hs;
			hl = hs + pool_size;
		}

		heap(const heap &) = delete;

		~heap() = default;

		void *malloc(size_t size)
		{
			// Try to allocate.
			byte *ptr = allocate(size);
			// If successed,return
			if (ptr != nullptr)
				return reinterpret_cast<void *>(ptr);
			// Compress the memory spaces
			compress();
			// Try to allocate again.
			ptr = allocate(size);
			// If successed,return.
			if (ptr != nullptr)
				return reinterpret_cast<void *>(ptr);
			else // There have no usable spaces,throw bad alloc exception.
				throw std::runtime_error("Bad alloc.");
		}

		void free(void *ptr)
		{
			free_list.push_back(reinterpret_cast<byte *>(ptr) - sizeof(size_t));
		}

		size_t size_of(void *ptr)
		{
			return get_size(reinterpret_cast<byte *>(ptr));
		}

		template<typename T, typename...ArgsT>
		T *allocate(ArgsT &&...args)
		{
			T *ptr = reinterpret_cast<T *>(this->malloc(sizeof(T)));
			::new(ptr) T(std::forward<ArgsT>(args)...);
			return ptr;
		}

		template<typename T>
		void deallocate(T *ptr)
		{
			ptr->~T();
			free(reinterpret_cast<void *>(ptr));
		}
	};
}