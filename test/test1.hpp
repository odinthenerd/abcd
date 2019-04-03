#include <kvasir/abcd/abcd.hpp>

namespace test1 {
struct other_policy;

namespace abcd = ::kvasir::abcd;

struct has_meaning_of_life {};
struct my_policy;

// only the public policies public function are part of the policy based classes
// public interface
template <typename T>
struct my_public_policy : T {
  void foo() { agents(this)[abcd::index<my_policy>].i_++; }
};

// policies state and members are encapsulated
struct my_policy {
  int i_;
  template <typename T>
  using f = my_public_policy<T>; // public interface
  int meaning_of_life() {
    return 43; // damn off by one errors
  }
};

struct call_meaning_of_life_t {
  template <typename T>
  void operator()(T t) {
    t.meaning_of_life();
  }
};

template <typename T>
struct other_public_policy : T {
  void init() {}

  void cleanup() {}

  void bar() {
    auto& data = agents(this);
    data[abcd::index<my_policy>].i_++; // can access other policies data
    data[abcd::index<other_policy>].i_am_data =
        data[abcd::index<my_policy>].meaning_of_life();
    data.for_each(abcd::ability<has_meaning_of_life>, call_meaning_of_life_t{});
  }
};

struct other_policy {
  int i_am_data;
  template <typename T>
  using f = other_public_policy<T>; // public interface
  template <typename T>
  void init(T t){
      // allocate or something
  };

  template <typename T>
  void cleanup(T t){
      // deallocate or something
  };
};

struct my_allocator {
  template <typename T>
  using f = void; // allocator has no public policy

  // note different allocator model
  char* allocate(std::size_t size) { return nullptr; }

  void deallocate(char* p, std::size_t size) {}
};

struct is_allocator {};

} // namespace test1
namespace kvasir {
namespace abcd {
template <>
struct has_ability<::test1::my_allocator, ::test1::is_allocator>
    : std::true_type {};
template <>
struct has_ability<::test1::other_policy, requires_init_and_destruct>
    : std::true_type {};
template <>
struct has_ability<::test1::my_policy, ::test1::has_meaning_of_life>
    : std::true_type {};
} // namespace abcd
} // namespace kvasir

namespace test1 {
int run() {
  auto m = abcd::combine(abcd::interface<my_public_policy>,
      abcd::interface<other_public_policy>, my_policy{0}, other_policy{4},
      my_allocator{});

  m.foo();
  m.bar();
  // m.data is private
  // m.meaning_of_life() totally hidden

  return 0;
}
} // namespace test1
