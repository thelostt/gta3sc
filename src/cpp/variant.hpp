///
/// Variant : A Type Safe Union
///
/// http://eggs-cpp.github.io/variant/
/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4542.pdf (unrelated to eggs::variant)
///
///
#pragma once
#include <eggs/variant.hpp>
using namespace eggs::variants;

/// Visits the currently active object on the [Vs]ariants by using F.
template<typename F, typename ...Vs>
constexpr auto visit(F&& f, Vs&&... vs)
    -> decltype(eggs::variants::apply(std::forward<F>(f), std::forward<Vs>(vs)...))
{
    return eggs::variants::apply(std::forward<F>(f), std::forward<Vs>(vs)...);
}

/// Visits the currently active object on the [V]ariant by using F.
template<typename F, typename V>
constexpr auto visit_one(V&& v, F&& f)
    -> decltype(eggs::variants::apply(std::forward<F>(f), std::forward<V>(v)))
{
    return eggs::variants::apply(std::forward<F>(f), std::forward<V>(v));
}

/// Checks whether the currently active object on the Variant is of type T.
template<typename T, typename Variant>
constexpr bool is(const Variant& variant)
{
    return visit_one(variant, [](const auto& obj) {
        return std::is_same<T, std::decay_t<decltype(obj)>>::value;
    });
}
