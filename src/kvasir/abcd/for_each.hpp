#pragma once
#include "abilities.hpp"
#include "types.hpp"
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/bind.hpp>

namespace kvasir {
namespace abcd {
namespace detail {
using namespace boost::mp11;
template <typename A, typename C>
using filter_by_capability =
    mp_filter_q<mp_bind_back<has_ability, C>, mp_apply<mp_list, A>>;

template <typename A, typename F, typename... Ts>
void for_each_helper(A& access, F l, mp_list<Ts...>) {
  int r[] = {0, (l(access[index<Ts>]), 0)...};
  (void)r;
}
} // namespace detail
} // namespace abcd
} // namespace kvasir
