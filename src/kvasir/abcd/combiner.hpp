#pragma once

#include "types.hpp"
#include "access.hpp"
#include "for_each.hpp"
#include <tuple>

namespace kvasir {
    namespace abcd {
        template<typename... Ts>
        class combiner;

        namespace detail{
            struct call_destruct{
                template<typename T>
                void operator()(T& t){
                    t.cleanup();
                }
            };
            struct call_init{
                template<typename T>
                void operator()(T& t){
                    t.init();
                }
            };
            template<typename...Ts>
            struct deriver : Ts... {};

            using make_base = remove_if<same_as<void>,cfe<deriver>>;
        }


        template<typename... Ts>
        class combiner : public ::kvasir::mpl::call<detail::make_base, typename Ts::template f<combiner<Ts...>> ...> {
            std::tuple<Ts...> data;
            using data_type = std::tuple<Ts...>;
        public:
            combiner(std::tuple<Ts...> &&d) : data{std::move(d)} {
                for_each(access<combiner>(data),ability<requires_init_and_destruct>,detail::call_init{});
            }

            friend class access<combiner>;

            ~combiner() {
                for_each(access<combiner>(data),ability<requires_init_and_destruct>,detail::call_destruct{});
            }
        };

        //factory
        template<typename...Ts>
        abcd::combiner<Ts...> combine(Ts...args){
            return std::tuple<Ts...>{args...};
        }
    }
}