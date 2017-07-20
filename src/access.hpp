#pragma once

#include "types.hpp"
#include <tuple>
#include "kvasir/mpl/mpl.hpp"

namespace kvasir {
    namespace abcd {
        using namespace ::kvasir::mpl;
        namespace detail {
            template<typename T, typename U>
            using index_from_tuple = uint_<(call<unpack<size<>>,U>::value - call<unpack<find_if<same_as<T>,size<>>>,U>::value)>;
        };

        template<typename T>
        struct access {
            decltype(T::data) &data;

            template<typename U>
            access(U *p):data{static_cast<T *>(p)->data} {}

            template<typename U>
            U &operator[](index_t <U>) {
                return std::get<detail::index_from_tuple<U,decltype(T::data)>::value>(data);
            }
        };
    }
}