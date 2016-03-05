#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>
#include <cppformat/format.h>
#include "cxx17/any.hpp"
#include "cxx17/variant.hpp"
#include "cxx17/optional.hpp"
#include "cxx17/filesystem.hpp"

// TODO make all maps and such be transparently comparable
// TODO transform lots of structs into class; make members private;
// TODO better organization of this (stdinc.h), perhaps put stuff in other utility headers

struct Var;
struct Script;
struct CodeGenerator;

using std::shared_ptr;
using std::weak_ptr;

struct GameConfig
{
    bool use_half_float = false;
    bool has_text_label_prefix = false;
};

struct iless
{
    using is_transparent = std::bool_constant<true>;

    bool operator()(const std::string& left, const std::string& right) const
    {
        return this->operator()(left.c_str(), right.c_str());
    }

    bool operator()(const char* left, const std::string& right) const
    {
        return this->operator()(left, right.c_str());
    }

    bool operator()(const std::string& left, const char* right) const
    {
        return this->operator()(left.c_str(), right);
    }

    bool operator()(const char* left, const char* right) const
    {
        return (_stricmp(left, right) < 0);
    }
};

struct iequal_to
{
    using is_transparent = std::true_type;

    bool operator()(const std::string& left, const std::string& right) const
    {
        return this->operator()(left.c_str(), right.c_str());
    }

    bool operator()(const char* left, const std::string& right) const
    {
        return this->operator()(left, right.c_str());
    }

    bool operator()(const std::string& left, const char* right) const
    {
        return this->operator()(left.c_str(), right);
    }

    bool operator()(const char* left, const char* right) const
    {
        return (_stricmp(left, right) == 0);
    }
};

template<typename Functor>
class scope_guard final
{
public:
    static_assert(std::is_nothrow_move_constructible<Functor>::value, "Functor must be NoThrow MoveConstructible");

    scope_guard(Functor f)
        : fun(std::move(f)), dismissed(false)
    {}

    ~scope_guard() noexcept
    {
        this->execute();
    }

    scope_guard(const scope_guard&) = delete;
    scope_guard(scope_guard&& rhs) noexcept
        : dismissed(rhs.dismissed), fun(std::move(rhs.fun))
    {
        rhs.dismiss();
    }

    scope_guard& operator=(scope_guard const&) = delete;
    scope_guard& operator=(scope_guard&& rhs) noexcept
    {
        this->fun = std::move(fun);
        this->dismissed = rhs.dismissed;
        rhs.dismiss();
    }

    void dismiss() noexcept
    {
        this->dismissed = true;
    }

    void execute() noexcept
    {
        if(!this->dismissed)
        {
            this->fun();
            this->dismiss();
        }
    }

private:
    Functor fun;
    bool    dismissed;
};

template<typename Functor>
inline auto make_scope_guard(Functor f) -> scope_guard<Functor>
{
    return scope_guard<Functor>(std::move(f));
}





struct broken_contract : std::runtime_error
{
    broken_contract(const char* msg)
        : std::runtime_error(msg)
    {}
};

struct unreachable_exception : std::runtime_error
{
    unreachable_exception(const char* msg)
        : std::runtime_error(msg)
    {}
};

// http://stackoverflow.com/a/19343239/2679626
#define STRINGIFY_DETAIL(x) #x
#define STRINGIFY(x) STRINGIFY_DETAIL(x)

/// I.6: Prefer Expects() for expressing preconditions.
/// See https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Ri-expects
#define Expects(cond) \
    ((cond)? void(0) : throw broken_contract("Precondition failure at  " __FILE__ "(" STRINGIFY(__LINE__) "): " #cond "."))

/// I.8: Prefer Ensures() for expressing postconditions.
/// See https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Ri-ensures
#define Ensures(cond) \
    ((cond)? void(0) : throw broken_contract("Postcondition failure at  " __FILE__ "(" STRINGIFY(__LINE__) "): " #cond "."))

/// Unreachable code must be marked with this.
#define Unreachable() \
    (throw unreachable_exception("Unreachable code reached at " __FILE__ "(" STRINGIFY(__LINE__) ")."))

/// Assert is used only for debugging purposes, release builds do not contains its checks.
#include <cassert>
