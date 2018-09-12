#pragma once
#include <typeinfo>
#include <typeindex>
#include <stdexcept>

namespace variant_impl {
	using byte_t=unsigned char;
	template<typename...ArgsT>struct template_args_iterator;
	template<typename T>struct template_args_iterator<T> {
		static constexpr std::size_t get_max_size(std::size_t size=0)
		{
			if(sizeof(T)>size)
				return sizeof(T);
			else
				return size;
		}
		static void copy_data(const std::type_index& ti, byte_t* src, byte_t* dest)
		{
			if(ti==typeid(T))
				::new (dest) T(*reinterpret_cast<T*>(src));
			else
				throw std::logic_error("Internal Error: No matching types.");
		}
		static void destroy_data(const std::type_index& ti, byte_t* src)
		{
			if(ti==typeid(T))
				reinterpret_cast<T*>(src)->~T();
			else
				throw std::logic_error("Internal Error: No matching types.");
		}
	};
	template<typename T, typename...ArgsT>struct template_args_iterator<T,ArgsT...> {
		static constexpr std::size_t get_max_size(std::size_t size=0)
		{
			if(sizeof(T)>size)
				return template_args_iterator<ArgsT...>::get_max_size(sizeof(T));
			else
				return template_args_iterator<ArgsT...>::get_max_size(size);
		}
		static void copy_data(const std::type_index& ti, byte_t* src, byte_t* dest)
		{
			if(ti==typeid(T))
				::new (dest) T(*reinterpret_cast<T*>(src));
			else
				template_args_iterator<ArgsT...>::copy_data(ti, src, dest);
		}
		static void destroy_data(const std::type_index& ti, byte_t* src)
		{
			if(ti==typeid(T))
				reinterpret_cast<T*>(src)->~T();
			else
				template_args_iterator<ArgsT...>::destroy_data(ti, src);
		}
	};
	struct monostate final {};
	template<typename...ArgsT>
	class variant final {
		using template_iterator_t=template_args_iterator<monostate, ArgsT...>;
		std::type_index current_type=typeid(monostate);
		byte_t data_container[template_iterator_t::get_max_size()];
	public:
		variant()
		{
			::new (data_container) monostate;
		}
		variant(const variant& var)
		{
			template_iterator_t::copy_data(var.current_type,var.data_container,data_container);
			current_type=var.current_type;
		}
		template<typename T>variant(const T& t)
		{
			::new (data_container) T(t);
			current_type=typeid(T);
		}
		~variant()
		{
			template_iterator_t::destroy_data(current_type, data_container);
		}
		variant& operator=(const variant& var)
		{
			if(&var!=this) {
				template_iterator_t::destroy_data(current_type, data_container);
				template_iterator_t::copy_data(var.current_type,var.data_container,data_container);
				current_type=var.current_type;
			}
			return *this;
		}
		template<typename T>
		variant& operator=(const T& t)
		{
			template_iterator_t::destroy_data(current_type, data_container);
			::new (data_container) T(t);
			current_type=typeid(T);
			return *this;
		}
		const std::type_index& type() const
		{
			return current_type;
		}
		template<typename T>T& get()
		{
			if(current_type==typeid(T))
				return *reinterpret_cast<T*>(data_container);
			else
				throw std::logic_error("Type does not match.");
		}
		template<typename T>const T& get() const
		{
			if(current_type==typeid(T))
				return *reinterpret_cast<T const*>(data_container);
			else
				throw std::logic_error("Type does not match.");
		}
		template<typename T>operator T&()
		{
			return get<T>();
		}
		template<typename T>operator const T&() const
		{
			return get<T>();
		}
	};
}