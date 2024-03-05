#ifndef OVERLOAD
#define OVERLOAD

namespace Powder::Util
{

// Utility class for pattern matching std::variant instances with std::visit
template <typename... Types> struct Overloaded : Types...
{
    using Types::operator()...;
};

// this deduction guide is not needed since C++20
template <typename... Types> Overloaded(Types...) -> Overloaded<Types...>;

} // namespace Powder::Util

#endif // OVERLOAD