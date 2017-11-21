#pragma once

#include "types.hpp"
#include "abilities.hpp"
#include "optional.hpp"
#include "for_each.hpp"
#include <tuple>
#include "kvasir/mpl/mpl.hpp"

namespace kvasir {
    namespace abcd {
        using namespace ::kvasir::mpl;
        namespace detail {
            template<typename T, typename U>
            using index_from_tuple = uint_<(call<unpack<size<>>,U>::value - call<unpack<find_if<same_as<T>,size<>>>,U>::value)>;
            template<typename T, typename U>
            using index_from_ability = uint_<(call<unpack<size<>>,U>::value - call<unpack<find_if<push_back<T,cfe<has_ability>>,size<>>>,U>::value)>;
        };

        template<typename T>
        struct access {
            template<typename U>
            U &operator[](index_t <U>) {
                return std::get<detail::index_from_tuple<U,typename T::data_type>::value>(static_cast<T*>(this)->data);
            }
            template<typename U>
            auto &operator[](ability_t <U>) {
                return std::get<detail::index_from_ability<U,typename T::data_type>::value>(static_cast<T*>(this)->data);
            }
            template<typename C, typename FT, typename FF>
            void optional(ability_t<C> a, FT lt, FF lf) {
                detail::optional_impl(detail::ability_in_tuple<C,T>, *this, a, lt, lf);
            }

            template<typename C, typename F>
            void for_each(ability_t <C>, F l) {
                detail::for_each_helper(*this, l, detail::filter_by_capability<T, C>{});
            }
        };
    }
}