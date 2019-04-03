#ifndef EXTEND_HELPER_HPP_INCLUDED
#define EXTEND_HELPER_HPP_INCLUDED

//  Copyright 2019 hhggit.
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/bind.hpp>

namespace kvasir {
namespace abcd {
namespace extend_helper {

template <template <class...> class... Fs>
struct flist {};

template <template <class...> class... Fs>
using qlist = boost::mp11::mp_list<boost::mp11::mp_quote<Fs>...>;

namespace detail {
template <template <template <class...> class...> class, class>
struct fl_apply_impl;

template <template <template <class...> class...> class FF,
    template <template <class...> class...> class LF,
    template <class...> class... Fs>
struct fl_apply_impl<FF, LF<Fs...>> {
  using type = FF<Fs...>;
};
} // namespace detail

template <template <template <class...> class...> class FF, class FL>
using fl_apply = typename detail::fl_apply_impl<FF, FL>::type;

template <class FL>
using to_qlist = fl_apply<qlist, FL>;

namespace detail {
template <template <template <class...> class...> class, class>
struct ql_apply_impl;

template <template <template <class...> class...> class FF,
    template <class...> class... Fs>
struct ql_apply_impl<FF, qlist<Fs...>> {
  using type = FF<Fs...>;
};
} // namespace detail

template <template <template <class...> class...> class FF, class QL>
using ql_apply = typename detail::ql_apply_impl<FF, QL>::type;

template <class QL>
using to_flist = ql_apply<flist, QL>;

namespace detail {
// take_until<L<A, B, C, D, C, E>, C> => L<A, B, C>
template <class L, class T>
using take_until =
    boost::mp11::mp_take_c<L, boost::mp11::mp_find<L, T>::value + 1>;
} // namespace detail

template <class B, class... Ds>
struct extend {
  using extend_root = B;
  using extend_dl   = boost::mp11::mp_list<Ds...>;

  // with_ql<qlist<M1, M2>>  =>  M2<M1<B, Ds...>, Ds...>
  template <class QL>
  using with_ql = boost::mp11::mp_reverse_fold_q<boost::mp11::mp_reverse<QL>, B,
      boost::mp11::mp_bind_back<boost::mp11::mp_invoke, Ds...>>;

  // with_t<M1, M2>  =>  M2<M1<B, Ds...>, Ds...>
  template <template <class...> class... Ms>
  using with_t = with_ql<qlist<Ms...>>;

  template <template <class...> class... Ms>
  struct with : with_t<Ms...> {
    using extend_type = extend;
    using extend_fl   = flist<Ms...>;

    template <template <class...> class M>
    using base_t = with_ql<
        detail::take_until<to_qlist<extend_fl>, boost::mp11::mp_quote<M>>>;

    template <template <class...> class M>
    base_t<M>& as() {
      return *this;
    }

    template <template <class...> class M>
    const base_t<M>& as() const {
      return *this;
    }
  };
};

template <class... Ds>
struct multi_extend {
  using extend_dl = boost::mp11::mp_list<Ds...>;

  template <template <class...> class... Ms>
  struct with : Ms<Ds...>... {
    using extend_type = multi_extend;
    using extend_fl   = flist<Ms...>;

    template <template <class...> class M>
    using base_t =
        std::enable_if_t<boost::mp11::mp_contains<to_qlist<extend_fl>,
                             boost::mp11::mp_quote<M>>::value,
            M<Ds...>>;

    template <template <class...> class M>
    base_t<M>& as() {
      return *this;
    }

    template <template <class...> class M>
    const base_t<M>& as() const {
      return *this;
    }
  };
};

} // namespace extend_helper
} // namespace abcd
} // namespace kvasir
#endif // EXTEND_HELPER_HPP_INCLUDED
