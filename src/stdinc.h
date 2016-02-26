#pragma once
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

// TODO make all maps and such be transparently comparable

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
