#pragma once
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "cxx17/any.hpp"
#include "cxx17/variant.hpp"
#include "cxx17/optional.hpp"
#include "cxx17/filesystem.hpp"
#include "thirdparty/cppformat/format.h"

// TODO make all maps and such be transparently comparable
// TODO better organization of this (stdinc.h), perhaps put stuff in other utility headers

struct Var;
struct Script;
struct CodeGenerator;

using std::shared_ptr;
using std::weak_ptr;

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
