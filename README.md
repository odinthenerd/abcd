# abcd
Agent Based Class Design is a paradigm similar to policy based class design but somewhat more generic. This library provides an implementation of a generic agent composition class and tools to help the user implement agents.

## what are agents?
There are several meanings for the term "agent", even within software development. In the sense of this library an agent is an object which is capable of performing a small set (preferably one) of tasks. In order to perform a task an agent may rely on the abilities of other agents. Each agent must publish its abilities, other agents use this information in order to find other agents which they rely on. Any composition of agents is valid if the abilities required by each agent is provided by one of the other agents in the composition. 

Within a composition agents live in a private arena of their own where they can find eachother either based on concrete type or ability and access eachothers public interfaces freely. In order to bridge the gap between this private arena and the outside world one or multiple public 'inteface' classes can be provided which can acess the private arena of agents and who's public interface is included in the public interace of the composition.

The user can also add abilities to a composition of agents. This allows elegant arbitrary nesting of compositions of agents acting themselves as agents within other compositions.


### I want to see code
A naive attempt to express the [wikipedia example of policy based class design](https://en.wikipedia.org/wiki/Policy-based_design) in terms of agent based class design:
```
#include <iostream>
#include <string>

struct print_capable{};
struct hello_capable{};

template<typename T>
struct run_it {
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
    struct has_capability<OutputPolicyWriteToCout,print_capable> : std::true_type{};
    template<>
    struct has_capability<LanguagePolicyEnglish,hello_capable> : std::true_type{};
    template<>
    struct has_capability<LanguagePolicyGerman,hello_capable> : std::true_type{};
}}

using namespace kvasir;
 
int main()
{
    /* Example 1 */
    auto hello1 = abcd::compose(abcd::interface<run_it>,OutputPolicyWriteToCout{},LanguagePolicyEnglish{});
    hello1.run(); // prints "Hello, World!"
 
    /* Example 2
     * Does the same, but uses another language agent */
    auto hello2 = abcd::compose(abcd::interface<run_it>,OutputPolicyWriteToCout{},LanguagePolicyGerman{});
    hello2.run(); // prints "Hallo Welt!"
}
```
This is a bit more verbose than the policy based class design version, however it is also more powerfull. For example if one of the agents needs to be initialized with state we can easily do that:
```
struct OutputPolicyWriteToOstream
{
    std::ostream& out;
    template<typename MessageType>
    void operator()(MessageType const &message) const
    {
        out << message << std::endl;
    }
};

auto hello = abcd::compose(abcd::interface<run_it>,OutputPolicyWriteToOstream{std::cout}, LanguagePolicyEnglish{});
```
We can also deal with variadic amounts of a given policy:
```
template<typename T>
struct run_it_public {
    // Behaviour method added to the public interface
    void run() const
    {
        auto agents_ = agents(this);
        for_each(this,ability<hello_capable>,[&](auto a){
                agents_[ability<print_capable>](a()+" ");
            });
    }
};


auto hello = abcd::compose(abcd::interface<run_it>,OutputPolicyWriteToOstream{std::cout},
        LanguagePolicyEnglish{}, LanguagePolicyGerman{});
hello1.run(); // prints "Hello, World! Hallo Welt!"
```
In other words in contrast to policy based class design we have potentially looser coupling, better reuseability and a more well defined as well as flexible interaction between agents. 

