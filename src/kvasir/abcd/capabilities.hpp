#pragma once
#include <type_traits>
namespace kvasir {
    namespace abcd {
        template<typename Agent, typename Capability>
        struct has_ability : std::false_type {
        };

        ///require this capability to be called in two phase init
        struct requires_init_and_destruct {
        };
    }
}