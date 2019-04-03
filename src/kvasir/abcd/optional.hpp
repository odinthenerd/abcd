#pragma once
#include "abilities.hpp"
#include "types.hpp"
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/bind.hpp>

namespace kvasir {
namespace abcd {
namespace detail {
using namespace boost::mp11;
template <typename T, typename C, typename FT, typename FF>
void optional_impl(const mp_true, T& base, ability_t<C> a, FT lt, FF lf) {
  lt(base[a]);
};
template <typename T, typename C, typename FT, typename FF>
void optional_impl(const mp_false, T&, ability_t<C>, FT, FF lf) {
  lf();
};
template <typename T, typename U>
constexpr mp_any_of_q<U, mp_bind_back<has_ability, T>> ability_in_tuple{};
} // namespace detail
} // namespace abcd
} // namespace kvasir
