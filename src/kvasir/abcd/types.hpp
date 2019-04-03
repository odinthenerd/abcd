#pragma once

namespace kvasir {
namespace abcd {

// index by type
template <typename T>
struct index_t {};
template <typename T>
constexpr index_t<T> index{};

// index by capability
template <typename T>
struct ability_t {};

template <typename T>
constexpr ability_t<T> ability{};

template <template <typename> class F>
struct wrap_trivial_public_interface {
  template <typename T>
  using f = F<T>;
};

} // namespace abcd
} // namespace kvasir
