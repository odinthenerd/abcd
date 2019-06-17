#pragma once

#include "types.hpp"
#include "abilities.hpp"
#include "optional.hpp"
#include "for_each.hpp"
#include <tuple>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/bind.hpp>

namespace kvasir {
    namespace abcd {
        namespace detail {
            using namespace boost::mp11;
            template<typename T, typename U>
            using index_from_tuple = mp_find<U, T>;

            template<typename T, typename U>
            using index_from_ability = mp_find_if_q<U,mp_bind_back<has_ability,T>>;
        };

        template<typename T>
        struct access {
            template<typename U>
            U &operator[](index_t <U>) {
                return std::get<detail::index_from_tuple<U,typename T::data_type>::value>(static_cast<T*>(this)->data);
            }
            template<typename U>
            auto &operator[](ability_t <U>) {
                return std::get<detail::index_from_ability<U,typename T::data_type>::value>(static_cast<T*>(this)->data);
            }
            template<typename C, typename FT, typename FF>
            void optional(ability_t<C> a, FT lt, FF lf) {
                detail::optional_impl(detail::ability_in_tuple<C,T>, *this, a, lt, lf);
            }

            template<typename C, typename F>
            void for_each(ability_t <C>, F l) {
                detail::for_each_helper(*this, l, detail::filter_by_capability<T, C>{});
            }


            template<typename U>
            const U &operator[](index_t <U>) const {
                return std::get<detail::index_from_tuple<U,typename T::data_type>::value>(static_cast<const T*>(this)->data);
            }
            template<typename U>
            const auto &operator[](ability_t <U>) const {
                return std::get<detail::index_from_ability<U,typename T::data_type>::value>(static_cast<const T*>(this)->data);
            }
            template<typename C, typename FT, typename FF>
            void optional(ability_t<C> a, FT lt, FF lf) const {
                detail::optional_impl(detail::ability_in_tuple<C,T>, *this, a, lt, lf);
            }
            template<typename C, typename F>
            void for_each(ability_t <C>, F l) const {
                detail::for_each_helper(*this, l, detail::filter_by_capability<T, C>{});
            }
        };

        template<typename T>
        access<T> &agents(access<T> *self) {
            return *self;
        }

        template<typename T>
        const access<T> &agents(const access<T> *self) {
            return *self;
        }

        template<typename T, typename C, typename F>
        void for_each(access<T> *self, ability_t<C> a, F l) {
            self->for_each(a, l);
        }

        template<typename T, typename C, typename F>
        void for_each(const access<T> *self, ability_t<C> a, F l) {
            self->for_each(a, l);
        }
    }
}
