#pragma once
#include <cstddef>
#include <tuple>

namespace cov {
    using byte_t=unsigned char*;

    template<std::size_t N> struct storage_helper { byte_t* mem_ptr=nullptr; };
}

#define cov_bind_member(type, member, address) cov::storage_helper<offsetof(type, member)>{address}

template<std::size_t ...offsets>
auto cov_make_field(cov::storage_helper<offsets>...members)
{
    return std::tuple<cov::storage_helper<offsets>...>(std::move(members)...);
}

namespace cov {
    template<typename T, std::size_t offset, std::size_t ...offsets>
    T& access_field_helper(const std::tuple<storage_helper<offsets>...>& field)
    {
        byte_t* address=std::get<storage_helper<offset>>(field).mem_ptr;
        return *reinterpret_cast<T*>(address);
    }
}

#define cov_access_field(type, member, field) cov::access_field_helper<decltype(type::member), offsetof(type, member)>(field)