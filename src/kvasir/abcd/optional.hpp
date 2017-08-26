#pragma once
#include "abilities.hpp"
#include "types.hpp"
#include "access.hpp"
#include "kvasir/mpl/mpl.hpp"

namespace kvasir{
    namespace abcd{
		namespace detail{
			using namespace mpl;
			template<template<typename> class B, typename T, typename C, typename FT, typename FF>
			void optional_impl(const bool_<true>,B<T> *base, ability_t<C> a, FT lt, FF lf){
				lt(access<T>(base)[a]);
			};
			template<typename C, typename FT, typename FF>
			void optional_impl(const bool_<false>,void*, ability_t<C>, FT, FF lf){
				lf();
			};
			template<typename T, typename U>
			constexpr call<unpack<find_if<push_back<T,cfe<has_ability>>,always<bool_<true>>,always<bool_<false>>>>,U> has_ability{};
		}
 		template<template<typename> class B, typename T, typename C, typename FT, typename FF>
		void optional(B<T> *base, ability_t<C> a, FT lt, FF lf) {
			optional_impl(detail::has_ability<C,T>, base, a, lt, lf);
		}
    }
}