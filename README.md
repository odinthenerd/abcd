# abcd
Agent Based Class Design is a paradigm similar to polic based class design but somewhat more generic. This library provides an implementation of a generic agent composition class and tools to help the user implement agents.

## what are agents?
There are several meanings for the term "agent", even within software development. In the sense of this library an agent is an object which is capable of performing a small set (preferably one) of tasks. In order to perform a task an agent may rely on other agents. Each agent must publish its capabilities, other agents use this information in order to find capabilities which they rely on. Any composition of agents is valid if the capabilities required by each agent is provided by one of the other agents in the composition. 
Concrete agent implementations consist of a private portion which may contain state and an optional public interface portion which cannot contain state but can access the private state of all other agents in the composition. The public interface of a composition of agents consists of an aggregation of all the public interface portions of all agents. Each action originates in a call to a function in the public interface which in turn may select one or many agents based on their published capabilities and call into them in order to perform the task.

### I want to see code
A niece attempt to express the [wikipedia example of policy based class design](https://en.wikipedia.org/wiki/Policy-based_design) in terms of agent based class design:
```
#include <iostream>
#include <string>

struct print_capable{};
struct hello_capable{};

template<typename T>
struct run_it_public {
    // Behaviour method added to the public interface
    void run() const
    {
        agents(this)[capavility_v<print_capable>](agents(this)[capavility_v<hello_capable>]());
    }
};
 
struct OutputPolicyWriteToCout
{
    template<typename T>
    using f = void; //no public interface

    template<typename MessageType>
    void operator()(MessageType const &message) const
    {
        std::cout << message << std::endl;
    }
};
 
struct LanguagePolicyEnglish
{
    template<typename T>
    using f = void; //no public interface

    std::string operator()() const
    {
        return "Hello, World!";
    }
};

struct LanguagePolicyGerman
{
    template<typename T>
    using f = void; //no public interface

    std::string operator()() const
    {
        return "Hallo Welt!";
    }
};

using run_it = kvasir::abcd::wrap_trivial_public_interface<run_it_public>;

namespace kvasir{ namespace abcd {
    template<>
    struct has_capability<OutputPolicyWriteToCout,print_capable> : std::true_type{};
    template<>
    struct has_capability<LanguagePolicyEnglish,hello_capable> : std::true_type{};
    template<>
    struct has_capability<LanguagePolicyGerman,hello_capable> : std::true_type{};
}}
 
int main()
{
    /* Example 1 */
    auto hello1 = kvasir::abcd::compose(OutputPolicyWriteToCout{},LanguagePolicyEnglish{},run_it{});
    hello1.run(); // prints "Hello, World!"
 
    /* Example 2
     * Does the same, but uses another language agent */
    auto hello2 = kvasir::abcd::compose(OutputPolicyWriteToCout{},LanguagePolicyGerman{},run_it{});
    hello2.run(); // prints "Hallo Welt!"
}
```
This is a bit more verbose than the policy based class design version, however it is also more powerfull. For example if one of the agents needs to be initialized with state we can easily do that:
```
struct OutputPolicyWriteToOstream
{
    std::ostream& out;
    template<typename T>
    using f = void; //no public interface

    template<typename MessageType>
    void operator()(MessageType const &message) const
    {
        out << message << std::endl;
    }
};

auto hello = kvasir::abcd::compose(OutputPolicyWriteToOstream{std::cout}, LanguagePolicyEnglish{},
        run_it{});
```
We can also deal with variadic amounts of a given policy:
```
template<typename T>
struct run_it_public {
    // Behaviour method added to the public interface
    void run() const
    {
        auto agents_ = agents(this);
        for_each(agents_,capavility_v<hello_capable>,[&](auto a){
                agents_[capavility_v<print_capable>](a()+" ");
            });
    }
};


auto hello = kvasir::abcd::compose(OutputPolicyWriteToOstream{std::cout},
        LanguagePolicyEnglish{}, LanguagePolicyGerman{}, run_it{});
hello1.run(); // prints "Hello, World! Hallo Welt!"
```
In other words in contrast to policy based class design we have potentially looser coupling, better reuseability and a more well defined as well as flexible interaction between agents. 

