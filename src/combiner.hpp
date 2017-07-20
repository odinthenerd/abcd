#pragma once

#include "types.hpp"
#include "access.hpp"
#include <tuple>

namespace abcd{
    //generic agent based class design combiner,
//only one generic pattern for all instances
    template<typename... Ts>
    class combiner : public Ts::template f<combiner<Ts...>>...{
        std::tuple<Ts...> data;
    public:
        combiner(std::tuple<Ts...>&& d):data{std::move(d)}{
            //metaprogramming here to find agents that need two phase init
            //std::get<other_policy>(data).init(data);
        }
        friend class access<combiner>;
        ~combiner(){
            //metaprogramming here to find agents that need two phase init
            //std::get<other_policy>(data).cleanup(data);
        }
    };
}