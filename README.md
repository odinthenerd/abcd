# abcd
Agent-Based Class Design is a paradigm similar to policy-based class design but somewhat more generic. This library provides an implementation of a generic agent composition class and tools to help the user implement agents.

## what are agents?
There are several meanings for the term "agent", even within software development. In the sense of this library an agent is an object which is capable of performing a small set (preferably one) of tasks. In order to perform a task an agent may rely on the abilities of other agents. Each agent must publish its abilities, other agents use this information in order to find other agents which they rely on. Any composition of agents is valid if the abilities required by each agent is provided by one of the other agents in the composition. 

Within a composition agents live in a private arena of their own where they can find each other either based on concrete type or ability and access each others public interfaces freely. In order to bridge the gap between this private arena and the outside world one or multiple public 'interface' classes can be provided which can acess the private arena of agents and who's public interface is included in the public interface of the composition.

The user can also add abilities to a composition of agents. This allows elegant arbitrary nesting of compositions of agents acting themselves as agents within other compositions.


### I want to see code
A naive attempt to express the [wikipedia example of policy-based class design](https://en.wikipedia.org/wiki/Policy-based_design) in terms of agent-based class design:
```cpp
#include <iostream>
#include <string>

using kvasir::abcd::agents;
using kvasir::abcd::ability;

struct print_capable{};
struct hello_capable{};

template<typename T>
struct run_it : T {
    // Behaviour method added to the public interface
    void run() const
    {
        agents(this)[ability<print_capable>](agents(this)[ability<hello_capable>]());
    }
};

struct OutputPolicyWriteToCout
{
    template<typename MessageType>
    void operator()(MessageType const &message) const
    {
        std::cout << message << std::endl;
    }
};
 
struct LanguagePolicyEnglish
{
    std::string operator()() const
    {
        return "Hello, World!";
    }
};

struct LanguagePolicyGerman
{
    std::string operator()() const
    {
        return "Hallo Welt!";
    }
};

namespace kvasir{ namespace abcd {
    template<>
    struct has_ability<OutputPolicyWriteToCout,print_capable> : std::true_type{};
    template<>
    struct has_ability<LanguagePolicyEnglish,hello_capable> : std::true_type{};
    template<>
    struct has_ability<LanguagePolicyGerman,hello_capable> : std::true_type{};
}}

using namespace kvasir;
 
int main()
{
    /* Example 1 */
    auto hello1 = abcd::combine(abcd::interface<run_it>,OutputPolicyWriteToCout{},LanguagePolicyEnglish{});
    hello1.run(); // prints "Hello, World!"
 
    /* Example 2
     * Does the same, but uses another language agent */
    auto hello2 = abcd::combine(abcd::interface<run_it>,OutputPolicyWriteToCout{},LanguagePolicyGerman{});
    hello2.run(); // prints "Hallo Welt!"
}
```
[Run on godbolt.org](https://godbolt.org/z/5ajHYo)

This is a bit more verbose than the policy-based class design version, however it is also more powerfull. For example if one of the agents needs to be initialized with state we can easily do that:
```cpp
struct OutputPolicyWriteToOstream
{
    std::ostream& out;
    template<typename MessageType>
    void operator()(MessageType const &message) const
    {
        out << message << std::endl;
    }
};

namespace kvasir{ namespace abcd {
template<>
    struct has_ability<OutputPolicyWriteToOstream,print_capable> : std::true_type{};
}}
    
auto hello = abcd::combine(abcd::interface<run_it>,OutputPolicyWriteToOstream{std::cout}, LanguagePolicyEnglish{});
```
[Run on godbolt.org](https://godbolt.org/z/oL4O9b)

We can also deal with variadic amounts of a given policy:
```cpp
template<typename T>
struct run_it_public : T {
    // Behaviour method added to the public interface
    void run() const
    {
        auto& agents_ = agents(this);
        for_each(this,ability<hello_capable>,[&](auto a){
                agents_[ability<print_capable>](a()+" ");
            });
    }
};


auto hello = abcd::combine(abcd::interface<run_it>,OutputPolicyWriteToOstream{std::cout},
        LanguagePolicyEnglish{}, LanguagePolicyGerman{});
hello.run(); // prints "Hello, World! Hallo Welt!"
```
[Run on godbolt.org](https://godbolt.org/z/4ndURu)

In other words in contrast to policy-based class design we have potentially looser coupling, better reusability and a more well defined as well as flexible interaction between agents.

### A Little More In-depth
We have seen the `[]` operator and `for_each` function in use in the examples above.  These provide for interacting with the first agent having a capability and with any agent having a capability.  The `optional` function provides for the case where we may not have an agent with a capability.

#### The `[]` operator
In this library, the `[]` operator selects the first agent which matches the capability requested.  This means that if there are multiple agents registering the requested capability, only the first will be returned by this function.  The order that agents are combined into the collection can effect which is selected by this function.
```
struct highlander_capable{};

template<typename T>
struct highlander_it : T {
    // Behaviour method added to the public interface
    void highlander() const
    {
        // demonstrate call to speak method of first highlander_capable agent
        agents(this)[ability<highlander_capable>].speak();
    }
};

struct MacLeod
{
    void speak() const { std::cout << "There can be only one!\n"; }
};

struct TheKurgan
{
    void speak() const { std::cout << "Another time MacLeod!\n"; }
};

namespace kvasir{ namespace abcd {
    template<>
    struct has_ability<MacLeod,highlander_capable> : std::true_type{};
    template<>
    struct has_ability<TheKurgan,highlander_capable> : std::true_type{};
}}

auto shout = abcd::combine(abcd::interface<highlander_it>,MacLeod{},TheKurgan{});
shout.highlander(); // prints "There can be only one!"
auto shout2 = abcd::combine(abcd::interface<highlander_it>,TheKurgan{},MacLeod{});
shout2.highlander(); // prints "Another time MacLeod!"
```
[Run on godbolt.org](https://godbolt.org/z/FKQp3X)

Note that the `agent&` is returned by the `[]` operator.  Methods can be invoked with the `.method_name()` syntax and the agent's `operator()` can be invoked `()` as seen previously.
```
struct hypnotize_capable{};

template<typename T>
struct hypnotize_it : T {
    // Behaviour method added to the public interface
    void hypnotize() const
    {
        // demonstrate call to operator() of first hypnotize_capable agent
        agents(this)[ability<hypnotize_capable>]();
    }
};

struct Mugatu
{
    void operator()() const { std::cout << "Kill the prime minister of Malaysia!\n"; }
};

namespace kvasir{ namespace abcd {
    template<>
    struct has_ability<Mugatu,hypnotize_capable> : std::true_type{};
}}

auto subliminal = abcd::combine(abcd::interface<hypnotize_it>,MacLeod{},Mugatu{});
subliminal.hypnotize(); // prints "Kill the prime minister of Malaysia!"
```
[Run on godbolt.org](https://godbolt.org/z/ZKZhGK)

#### The `for_each` function
Collections that have multiple agents satisfying a capability may wish to act upon all of these agents.  The `for_each` function sequentially applies a function to all of the agents in the collection which statisfy the capability requirement.  Note: Use a lambda here to make your life easier. The lambda is not required to capture the environment.
```
template<typename T>
struct speak_it : T {
    // Behaviour method added to the public interface
    void speak() const
    {
        // demonstrate call to speak method of all highlander_capable agents
        auto &agents_ = agents(this);
        for_each(this, ability<highlander_capable>, [&](auto a){ a.speak(); });
    }
};

auto taunt = abcd::combine(abcd::interface<speak_it>,MacLeod{},TheKurgan{});
taunt.speak(); // prints "There can be only one!" followed by "Another Time MacLeod!"

```
[Run on godbolt.org](https://godbolt.org/z/szBk7B)

#### The `optional` function
Collections which may not have any agents satisfying a capability may still need to implement an interface depending on that capability.  The `optional` function allows for this case and alternatively executes a true case function or a fallback function.  Note: use lambdas here unless you like self inflicted pain.
```
struct relax_capable{};

template<typename T>
struct relax_it : T {
    // Behaviour method added to the public interface
    void relax() const
    {
        // demonstrate optional call to relax method or fallback
        agents(this).optional(ability<relax_capable>,
         [&](auto a){a.relax();},
         [](){std::cout << "I invented the Paino Key Necktie!\n"; });
    }
};

struct Frankie
{
    void relax() const { std::cout << "Relax! Don't do it!\n"; }
};

namespace kvasir{ namespace abcd {
    struct has_ability<Frankie,relax_capable> : std::true_type{};
}}

auto sing = abcd::combine(abcd::interface<relax_it>,Frankie{},MacLeod{});
sing.relax(); // prints "Relax! Don't do it!"
auto scream = abcd::combine(abcd::interface<relax_it>,Mugatu{},MacLeod{});
scream.relax(); // prints "I invented the Piano Key Necktie!"
```
[Run on godbolt.org](https://godbolt.org/z/44BgS8)
