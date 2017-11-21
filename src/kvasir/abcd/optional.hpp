#pragma once
#include "abilities.hpp"
#include "types.hpp"
#include "access.hpp"
#include "kvasir/mpl/mpl.hpp"

namespace kvasir{
    namespace abcd{
		namespace detail{
			using namespace mpl;
			template<typename T, typename C, typename FT, typename FF>
			void optional_impl(const bool_<true>,T& base, ability_t<C> a, FT lt, FF lf){
				lt(base[a]);
			};
			template<typename T, typename C, typename FT, typename FF>
			void optional_impl(const bool_<false>,T&, ability_t<C>, FT, FF lf){
				lf();
			};
			template<typename T, typename U>
			constexpr call<unpack<find_if<push_back<T,cfe<has_ability>>,always<bool_<true>>,always<bool_<false>>>>,U> ability_in_tuple{};
		}
    }
}