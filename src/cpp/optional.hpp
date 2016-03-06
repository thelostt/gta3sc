/// Library Fundamentals v1 : Optional
///
/// http://en.cppreference.com/w/cpp/experimental/optional
/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4480.html
///
#pragma once

#ifndef _CXX17_OPTIONAL_TYPE
#  ifdef __has_include
#    if __has_include(<optional>)                // Check for a standard library
#     include <optional>
#     define _CXX17_OPTIONAL_TYPE    std::optional
#     define _CXX17_NULLOPT_OBJECT   std::nullopt
#    elif __has_include(<experimental/optional>) // Check for an experimental version
#     include <experimental/optional>
#     define _CXX17_OPTIONAL_TYPE    std::experimental::optional
#     define _CXX17_NULLOPT_OBJECT   std::experimental::nullopt
/*
#    elif __has_include(<boost/optional.hpp>)    // Try with boost library..... No no no...
#     include <boost/optional.hpp>               // ...Let the user be explicit about using boost instead!
#     include <boost/version.hpp>
#     define _CXX17_OPTIONAL_TYPE    boost::optional
#     define _CXX17_NULLOPT_OBJECT   boost::none       // but it's none, not nullopt, what to do?
*/
#    endif
#  endif
#  ifndef _CXX17_OPTIONAL_TYPE           // Still undefined? Use our embeded version.
#    include <optional/optional.hpp>
#    define _CXX17_OPTIONAL_TYPE    std::experimental::optional
#    define _CXX17_NULLOPT_OBJECT   std::experimental::nullopt
#  endif
#endif

template<typename T> using optional = _CXX17_OPTIONAL_TYPE<T>;
constexpr decltype(_CXX17_NULLOPT_OBJECT) nullopt = _CXX17_NULLOPT_OBJECT;
