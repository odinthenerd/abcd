#include "../src/abcd.hpp"


struct other_policy;

struct has_meaning_of_life{};
struct my_policy;
namespace abcd{
    template<>
    struct has_capability<my_policy,has_meaning_of_life> : std::true_type{};
}
//only the public policies public function are part of the policy based classes public interface
template<typename T>
struct my_public_policy{
    void foo(){
        auto a = abcd::access<T>(this);
        a[abcd::index_t<my_policy>{}].i_++;
    }
};

//policies state and members are encapsulated
struct my_policy{
    int i_;
    template<typename T>
    using f = my_public_policy<T>;  //public interface
    int meaning_of_life(){
        return 43; //damn off by one errors
    }
};

struct call_meaning_of_life_t{
    template<typename T>
            void operator()(T t){
        t.meaning_of_life();
    }
};

template<typename T>
struct other_public_policy{
    void init(){}
    void cleanup(){}
    void bar(){
        auto data = abcd::access<T>(this);
        data[abcd::index_t<my_policy>{}].i_++;//can access other policies data
        data[abcd::index_t<other_policy>{}].i_am_data = data[abcd::index_t<my_policy>{}].meaning_of_life();
        data.for_each(abcd::capability_t<has_meaning_of_life>{},call_meaning_of_life_t{});
    }
};

namespace abcd {
    template<>
    struct has_capability<other_policy, abcd::requires_init_and_destruct> : std::true_type {
    };
}
struct other_policy{
    int i_am_data;
    template<typename T>
    using f = other_public_policy<T>;  //public interface
    template<typename T>
    void init(T t){
        //allocate or something
    };
    template<typename T>
    void cleanup(T t){
        //deallocate or something
    };
};

template<typename>
struct my_allocator_public{};

struct my_allocator{
    template<typename T>
    using f = my_allocator_public<T>;
    //note different allocator model
    char* allocate(std::size_t size){
        return nullptr;
    }
    void deallocate(char* p, std::size_t size){

    }
};
struct is_allocator{};

namespace abcd {
    template<>
    struct has_capability<my_allocator, is_allocator> : std::true_type {
    };
}

//factory
template<typename...Ts>
abcd::combiner<Ts...> make_my_class(Ts...args){
    //arbitrary plausibility checks here
    return std::tuple<Ts...>{args...};
}

int main(){
    auto m = make_my_class(my_policy{0}, other_policy{4}, my_allocator{});
    //m.foo();
    //m.bar();
    //m.data is private
    //m.meaning_of_life() totally hidden
}