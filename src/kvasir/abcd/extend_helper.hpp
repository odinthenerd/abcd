#pragma once

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

            template<template<class...> class... Fs>
            using qlist = boost::mp11::mp_list<boost::mp11::mp_quote<Fs>...>;

            namespace detail {
                template<template<template<class...> class...> class, class>
                struct fl_apply_impl;

                template<template<template<class...> class...> class FF, template<template<class...> class...> class LF, template<class...> class... Fs>
                struct fl_apply_impl<FF, LF<Fs...>> {
                    using type = FF<Fs...>;
                };
            }

            template<template<template<class...> class...> class FF, class FL>
            using fl_apply = typename detail::fl_apply_impl<FF, FL>::type;

            template<class FL>
            using to_qlist = fl_apply<qlist, FL>;

            template<class B, class... Ds>
            struct extend {
                // with_ql<qlist<M1, M2>>  =>  M2<M1<B, Ds...>, Ds...>
                template<class QL>
                using with_ql = boost::mp11::mp_reverse_fold_q<boost::mp11::mp_reverse<QL>, B, boost::mp11::mp_bind_back<boost::mp11::mp_invoke, Ds...>>;

                // with_t<M1, M2>  =>  M2<M1<B, Ds...>, Ds...>
                template<template<class...> class... Ms>
                using with_t = with_ql<qlist<Ms...>>;
            };
        }
    }
}
