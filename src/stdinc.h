#pragma once

#ifdef _MSC_VER
#   define _CRT_SECURE_NO_WARNINGS
#   define _SCL_SECURE_NO_WARNINGS
#endif

#include <cstdint>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <atomic>
#include <cppformat/format.h>
#include "cpp/any.hpp"
#include "cpp/variant.hpp"
#include "cpp/optional.hpp"
#include "cpp/filesystem.hpp"
#include "cpp/scope_guard.hpp"
#include "cpp/string_view.hpp"
#include "cpp/small_vector.hpp"
#include "cpp/icompare.hpp"
#include "cpp/contracts.hpp"
#include "cpp/file.hpp"

#pragma warning(push)
#pragma warning(disable : 4814) // warning: in C++14 'constexpr' will not imply 'const'; consider explicitly specifying 'const'
#include "cpp/expected.hpp"
#pragma warning(pop)

using std::shared_ptr;
using std::weak_ptr;
using dynamic_bitset = std::vector<bool>;

template<typename Value>
using transparent_set = std::set<Value, std::less<>>;

template<typename Key, typename Value>
using transparent_map = std::map<Key, Value, std::less<>>;

template<typename Key, typename Value>
using transparent_multimap = std::multimap<Key, Value, std::less<>>;

template<typename Key, typename Value>
using insensitive_map = std::map<Key, Value, iless>;

template<typename Key>
using insensitive_set = std::set<Key, iless>;

class SyntaxTree;
class ProgramContext;
class Options;
class Commands;
class CodeGenerator;
struct Command;
struct Var;
class Script;
class Scope;
class SymTable;
struct CompiledScmHeader;
class MultiFileHeaderList;
struct Label;

#ifndef _MSC_VER
#   define __debugbreak()
#endif

//////// TODO move the following to a 'common' header

/// IR for end of argument list used in variadic argument commands.
struct EOAL
{
};

/// Declared type of a variable.
enum class VarType : uint8_t
{
    Int,
    Float,
    TextLabel,
    TextLabel16,
};

// for future parallelism
template<typename IndexType, typename Functor>
inline void for_loop(IndexType begin, IndexType end, Functor functor)
{
    for(auto i = begin; i != end; ++i)
        functor(i);
}

inline std::string escape_string(const string_view& string, char quotes, bool push_quotes)
{
    std::string result;
    result.reserve(string.size() + (push_quotes? 2 : 0));

    if(push_quotes) result.push_back(quotes);
    for(auto& c : string)
    {
        switch(c)
        {
            case '\\': result += R"(\\")"; break;
            case '\n': result += R"(\n)"; break;
            case '\r': result += R"(\r)"; break;
            case '\t': result += R"(\t)"; break;
            default:
                if(c == quotes)
                    result.push_back('\\');
                result.push_back(c);
                break;
        }
    }
    if(push_quotes) result.push_back(quotes);

    return result;
}

inline string_view remove_quotes(const string_view& string)
{
    Expects(string.size() >= 2 && string.front() == '"' && string.back() == '"');
    return string_view(string.data() + 1, string.size() - 2);
}

// based off std::quoted
inline std::string make_quoted(const string_view& string, char quotes = '"')
{
    return escape_string(string, quotes, true);
}

inline char toupper_ascii(char c)
{
    if(c >= 'a' && c <= 'z')
        return c - ('a' - 'A');
    return c;
}
