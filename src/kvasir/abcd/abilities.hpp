#pragma once
#include <type_traits>
#include <kvasir/mpl/mpl.hpp>
namespace kvasir {
    namespace abcd {
        ///Use this to associate abilities with an agent without specializing traits
        template<typename...Ts>
        struct abilities_t {};

        template<typename...Ts>
        constexpr abilities_t<Ts...> abilities{};

        template<typename Agent, typename Capability>
        struct has_ability : mpl::false_ {
        };

        template<template<typename...> class W, typename...Ts, typename...Us, typename Ability>
        struct has_ability<W<abilities_t<Ts...>,Us...>,Ability> :
                mpl::call<mpl::find_if<mpl::same_as<Ability>,mpl::always<mpl::true_>,mpl::always<mpl::false_>>,Ts...>{};

        ///require this capability to be called in two phase init
        struct requires_init_and_destruct {
        };
    }
}