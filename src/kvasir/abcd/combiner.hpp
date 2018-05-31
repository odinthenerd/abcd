#pragma once

#include "types.hpp"
#include "access.hpp"
#include "for_each.hpp"
#include "interface.hpp"
#include <tuple>

namespace kvasir {
    namespace abcd {
        template<typename... Ts>
        class combiner;

        namespace detail{
            struct call_destruct{
                template<typename T>
                void operator()(T& t){
                    t.cleanup(t);
                }
            };
            struct call_init{
                template<typename T>
                void operator()(T& t){
                    t.init(t);
                }
            };

            template<bool B>
            struct recursive_derive{
                template<typename T, template<typename...> class P, template<typename...> class...Ps>
                using f = typename recursive_derive<(sizeof...(Ps)>0)>::template f<P<T>,Ps...>;
            };
            template<>
            struct recursive_derive<false>{
                template<typename T, template<typename...> class...Ps>
                using f = T;
            };

            template<typename T, typename...Ts>
            struct recursive_unpack_impl{
                    using type = T;
            };
            template<typename T, template<typename...> class...P0s,template<typename...> class...P1s,template<typename...> class...P2s,template<typename...> class...P3s,typename...Ps>
            struct recursive_unpack_impl<T, interface_t<P0s...>,interface_t<P1s...>,interface_t<P2s...>,interface_t<P3s...>, Ps...>{
                using type = typename recursive_unpack_impl<typename recursive_derive<((sizeof...(P0s)+sizeof...(P1s)+sizeof...(P2s)+sizeof...(P3s))>0)>::template f<T,P0s...,P1s...,P2s...,P3s...>,Ps...>::type;
            };

            template<typename T, typename...Ts>
            using recursive_unpack = typename recursive_unpack_impl<T,Ts...,interface_t<>,interface_t<>,interface_t<>,interface_t<>>::type;

            template<typename T>
            struct is_interface : mpl::false_{};

            template<template<typename...> class...Ps>
            struct is_interface<interface_t<Ps...>> : mpl::true_ {};

            using namespace mpl;

            template<typename T, typename...Ts>
            using make_interface = call<filter<cfe<is_interface>, push_front<access<T>,cfe<recursive_unpack>>>,Ts...>;

        }


        template<typename... Ts>
        class combiner : public detail::make_interface<combiner<Ts...>, Ts...> {
            std::tuple<Ts...> data;
            using data_type = std::tuple<Ts...>;
        public:
            combiner(std::tuple<Ts...> &&d) : data{std::move(d)} {
                this->for_each(ability<requires_init_and_destruct>,detail::call_init{});
            }

            friend struct access<combiner>;

            ~combiner() {
                this->for_each(ability<requires_init_and_destruct>,detail::call_destruct{});
            }
        };

        //factory
        template<typename...Ts>
        combiner<Ts...> combine(Ts...args){
            return std::tuple<Ts...>{args...};
        }
    }
}