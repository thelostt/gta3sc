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

// TODO make all maps and such be transparently comparable
// TODO transform lots of structs into class; make members private;

using std::shared_ptr;
using std::weak_ptr;
using dynamic_bitset = std::vector<bool>;

class SyntaxTree;
class ProgramContext;
class Options;
class Commands;
class CodeGenerator;
struct Command;
struct Var;
struct Script;
struct Scope;
struct SymTable;

template<typename Key, typename Value>
using transparent_map = std::map<Key, Value, std::less<>>;

template<typename Key, typename Value>
using transparent_multimap = std::multimap<Key, Value, std::less<>>;

template<typename Key, typename Value>
using insensitive_map = std::map<Key, Value, iless>;

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