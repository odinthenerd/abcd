#pragma once
#include "capabilities.hpp"

namespace kvasir{
    namespace abcd{
        namespace detail{
            using namespace mpl;
            template<typename A, typename C>
            using filter_by_capability = call<unpack<filter<push_back<C,cfe<has_capability>>>>,A>;
        }
        template<typename A, typename C, typename F, typename...Ts>
        void for_each(A access, capability_t <C>, F l, list<Ts...> filtered = detail::filter_by_capability<A,C>{}) {
            int r[] = {(l(access[capability_t<Ts>{}]),0)...};
            (void)r;
        }
    }
}