#pragma once

#include "types.hpp"
#include "access.hpp"
#include "for_each.hpp"
#include "interface.hpp"
#include <tuple>
#include "extend_helper.hpp"
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/bind.hpp>

namespace kvasir {
namespace abcd {
template <typename... Ts>
class combiner;

namespace detail {
struct call_destruct {
  template <typename T>
  void operator()(T& t) {
    t.cleanup(t);
  }
};
struct call_init {
  template <typename T>
  void operator()(T& t) {
    t.init(t);
  }
};

using namespace boost::mp11;
template <typename T>
struct is_interface : mp_false {};

template <template <typename...> class... Ps>
struct is_interface<interface_t<Ps...>> : mp_true {};

template <typename T>
using has_pub_interface_impl = is_interface<typename T::pub_interface>;

template <typename T>
using has_pub_interface = mp_eval_or<mp_false, has_pub_interface_impl, T>;

template <class T>
using to_qlist = extend_helper::to_qlist<typename T::pub_interface>;

template <typename T, typename... Ts>
using make_interface = typename extend_helper::extend<
    access<T>>::template with_ql<mp_apply<mp_append,
    mp_transform<to_qlist, mp_filter<has_pub_interface, mp_list<Ts...>>>>>;
} // namespace detail

template <typename... Ts>
class combiner : public detail::make_interface<combiner<Ts...>, Ts...> {
  std::tuple<Ts...> data;
  using data_type = std::tuple<Ts...>;

public:
  combiner(std::tuple<Ts...>&& d) : data{std::move(d)} {
    agents(this).for_each(
        ability<requires_init_and_destruct>, detail::call_init{});
  }

  friend struct access<combiner>;

  ~combiner() {
    agents(this).for_each(
        ability<requires_init_and_destruct>, detail::call_destruct{});
  }
};

// factory
template <typename... Ts>
combiner<Ts...> combine(Ts... args) {
  return std::tuple<Ts...>{args...};
}
} // namespace abcd
} // namespace kvasir
