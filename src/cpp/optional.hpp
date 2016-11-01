/// Library Fundamentals v1 : Optional
///
/// http://en.cppreference.com/w/cpp/experimental/optional
/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4480.html
///
#pragma once
#include <optional/optional.hpp>

template<typename T>
using optional = std::experimental::optional<T>;

using nullopt_t = std::experimental::nullopt_t;
constexpr nullopt_t nullopt = std::experimental::nullopt;

using bad_optional_access = std::experimental::bad_optional_access;
