#pragma once
#include <eggs/variant.hpp>
using namespace eggs::variants;

template<class F, class ...Vs>
constexpr auto visit(F&& f, Vs&&... vs)
    -> decltype(eggs::variants::apply(std::forward<F>(f), std::forward<Vs>(vs)...))
{
    return eggs::variants::apply(std::forward<F>(f), std::forward<Vs>(vs)...);
}

template<class F, class Vs>
constexpr auto visit_one(Vs&& vs, F&& f)
    -> decltype(eggs::variants::apply(std::forward<F>(f), std::forward<Vs>(vs)))
{
    return eggs::variants::apply(std::forward<F>(f), std::forward<Vs>(vs));
}

template<typename T, typename Variant>
constexpr bool is(const Variant& variant)
{
    return visit_one(variant, [](const auto& obj) {
        return std::is_same<T, std::decay_t<decltype(obj)>>::value;
    });
}


