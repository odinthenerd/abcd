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
            template<typename...Ts>
            struct deriver : Ts... {};

            template<typename T, typename U>
            struct make_interface_impl{  //default return an empty list because we don't need its type
                using type = list<>;
            };

            template<typename T, template<typename T> class ...Ts>
            struct make_interface_impl<T,interface_t<Ts...>>{
                    using type = list<Ts<T>...>;
            };

            //takes the derived class as a fixed parameter, filters
            //on interface types, unpacks their contents and
            //passes the provided T as the template parameter
            //changing them to types
            template<typename T>
            struct make_interface{
                template<typename...Ts>
                using f = call<join<cfe<deriver>>,typename make_interface_impl<T,Ts>::type...>;
            };
        }


        template<typename... Ts>
        class combiner : public ::kvasir::mpl::call<detail::make_interface<combiner<Ts...>>, Ts...> {
            std::tuple<Ts...> data;
            using data_type = std::tuple<Ts...>;
        public:
            combiner(std::tuple<Ts...> &&d) : data{std::move(d)} {
                for_each(access<combiner>(data),ability<requires_init_and_destruct>,detail::call_init{});
            }

            friend struct access<combiner>;

            ~combiner() {
                for_each(access<combiner>(data),ability<requires_init_and_destruct>,detail::call_destruct{});
            }
        };

        //factory
        template<typename...Ts>
        combiner<Ts...> combine(Ts...args){
            return std::tuple<Ts...>{args...};
        }
    }
}