#pragma once
#include "capabilities.hpp"
#include "types.hpp"
#include "kvasir/mpl/mpl.hpp"

namespace kvasir{
    namespace abcd{
        namespace detail{
            using namespace ::kvasir::mpl;
            template<typename A, typename C>
            using filter_by_capability = call<unpack<filter<push_back<C,cfe<has_capability>>>>,A>;
        }
		namespace detail {
			template<typename A, typename F, typename...Ts>
			void for_each_helper(A access, F l, ::kvasir::mpl::list<Ts...>) {
				int r[] = { 0,(l(access[index_t<Ts>{}]),0)... };
				(void)r;
			}
		}
        template<typename T, typename C, typename F>
        void for_each(access<T> access, capability_t <C>, F l) {
			detail::for_each_helper(access, l, detail::filter_by_capability<T, C>{});
        }
    }
}