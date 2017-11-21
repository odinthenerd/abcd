#pragma once
namespace kvasir {
    namespace abcd {
        ///Use this to associate abilities with an agent without specializing traits
        template<template<typename...> class...>
        struct interface_t {};

        template<template<typename...> class...Ts>
        constexpr interface_t<Ts...> interface{};

    }
}