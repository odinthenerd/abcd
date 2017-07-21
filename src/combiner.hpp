#pragma once

#include "types.hpp"
#include "access.hpp"
#include "for_each.hpp"
#include <tuple>

namespace kvasir {
    namespace abcd {
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
        }
        //generic agent based class design combiner,
//only one generic pattern for all instances
        template<typename... Ts>
        class combiner : public Ts::template f<combiner<Ts...>> ... {
            std::tuple<Ts...> data;
            using data_type = std::tuple<Ts...>;
        public:
            combiner(std::tuple<Ts...> &&d) : data{std::move(d)} {
                for_each(access<combiner>(this),capability_t<requires_init_and_destruct>{},detail::call_init{});
            }

            friend class access<combiner>;

            ~combiner() {
                for_each(access<combiner>(this),capability_t<requires_init_and_destruct>{},detail::call_destruct{});
            }
        };

        //factory
        template<typename...Ts>
        abcd::combiner<Ts...> combine(Ts...args){
            return std::tuple<Ts...>{args...};
        }
    }
}