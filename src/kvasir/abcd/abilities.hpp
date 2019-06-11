#pragma once
#include <type_traits>
#include <boost/mp11/algorithm.hpp>
namespace kvasir {
    namespace abcd {
        ///Use this to associate abilities with an agent without specializing traits
        template<typename...Ts>
        struct abilities_t {};

        template<typename...Ts>
        constexpr abilities_t<Ts...> abilities{};

        namespace detail {
            using namespace boost::mp11;

            template<typename Agent, typename Ability>
            struct has_ability_impl : mp_false {};

            template<template<typename...> class W, typename... Ts, typename... Us, typename Ability>
            struct has_ability_impl<W<abilities_t<Ts...>,Us...>,Ability> :
                    mp_eval_if<mp_contains<abilities_t<Ts...>, Ability>,mp_true,has_ability_impl,W<Us...>,Ability> {};

            template<template<typename...> class W, typename T, typename... Us, typename Ability>
            struct has_ability_impl<W<T,Us...>,Ability> : has_ability_impl<W<Us...>, Ability> {};
        }

        template<typename Agent, typename Capability>
        struct has_ability : detail::has_ability_impl<Agent,Capability> {};

        ///require this capability to be called in two phase init
        struct requires_init_and_destruct {
        };
    }
}
