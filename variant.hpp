#pragma once
#include <typeinfo>
#include <typeindex>
#include <stdexcept>

namespace variant_impl
{
    using byte_t=unsigned char;
    template<typename...ArgsT>struct template_args_container{};
    template<typename...ArgsT>struct template_args_iterator;
    template<typename T>struct template_args_iterator<T>
    {
        static constexpr std::size_t get_max_aligned_size(std::size_t size=0)
        {
            if(alignof(T)>size)
                return alignof(T);
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
    };
    template<typename T, typename...ArgsT>struct template_args_iterator<T,ArgsT...>
    {
        static constexpr std::size_t get_max_aligned_size(std::size_t size=0)
        {
            if(alignof(T)>size)
                return template_args_iterator<ArgsT...>::get_max_aligned_size(alignof(T));
            else
                return template_args_iterator<ArgsT...>::get_max_aligned_size(size);
        }
        static void copy_data(const std::type_index& ti, byte_t* src, byte_t* dest)
        {
            if(ti==typeid(T))
                ::new (dest) T(*reinterpret_cast<T*>(src));
            else
                template_args_iterator<ArgsT...>::copy_data(ti, src, dest);
        }
    };
    template<typename...ArgsT>
    class variant final
    {
        std::type_index current_type=typeid(void);
        byte_t data_container[template_args_iterator<ArgsT...>::get_max_aligned_size()];
        template<typename T>
		struct construct_helper {
            template<typename...Elements>
            static void construct(variant* _this, Elements&&...args)
            {
                ::new (_this->data_container) T(std::forward<Elements...>(args));
                _this->current_type=typeid(T);
            }
        };
    public:
        variant()=delete;
        template<typename T>variant(T&& t)
    };

    template<typename...ArgsT>
    struct variant<ArgsT...>::construct_helper<const variant<ArgsT...>>
    {
        static void construct(variant* _this, const variant& var)
        {

        }
    };
}