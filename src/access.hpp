#pragma once

#include "types.hpp"
#include <tuple>

namespace abcd{
    template<typename T>
    struct access{
        decltype(T::data)& data;
        template<typename U>
        access(U* p):data{static_cast<T*>(p)->data}{}
        template<typename U>
        decltype(std::get<U>(data))& operator[](index_t<U>){
            return std::get<U>(data);
        }
        template<typename C, typename Lambda>
        void for_each(capability_t<C>, Lambda l){
            //execute lambda with each capability
        }
    };
}