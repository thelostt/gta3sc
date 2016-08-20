#pragma once

#ifdef _MSC_VER
#   define _CRT_SECURE_NO_WARNINGS
#endif

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <algorithm>
#include <iterator>
#include <atomic>
#include <cppformat/format.h>
#include "cpp/any.hpp"
#include "cpp/variant.hpp"
#include "cpp/optional.hpp"
#include "cpp/filesystem.hpp"
#include "cpp/scope_guard.hpp"
#include "cpp/icompare.hpp"
#include "cpp/contracts.hpp"
#include "cpp/file.hpp"

// TODO make all maps and such be transparently comparable
// TODO transform lots of structs into class; make members private;
// TODO better organization of this (stdinc.h), perhaps put stuff in other utility headers

using std::shared_ptr;
using std::weak_ptr;
using dynamic_bitset = std::vector<bool>;

class SyntaxTree;
class ProgramContext;
struct Command;
struct Var;
struct Script;
struct Scope;
struct SymTable;
struct CodeGenerator;
struct CodeGeneratorBase;

#ifndef _MSC_VER
#   define __debugbreak()
#endif


// URGENT!!!!!! TODO TODO TODO!!!!!!!!!!!!! USE A PROPER DYNAMIC_BITSET LIBRARY AND REMOVE THIS!!!

template<typename OpFunctor>
inline dynamic_bitset& ops_dynamic_bitset(dynamic_bitset& a, const dynamic_bitset& b)
{
    Expects(a.size() == b.size());

    OpFunctor bitop;

    #if defined(_MSC_VER)
    auto a_begin = a._Myvec.begin();
    auto b_begin = b._Myvec.begin();
    auto a_end   = a._Myvec.end();
    #else
    auto a_begin = a.begin();
    auto b_begin = b.begin();
    auto a_end   = a.end();
    #endif

    for(; a_begin < a_end; ++a_begin, ++b_begin)
    {
        *a_begin = bitop(*a_begin, *b_begin);
    }

    return a;
}

inline dynamic_bitset& operator&=(dynamic_bitset& a, const dynamic_bitset& b)
{
    return ops_dynamic_bitset<std::bit_and<void>>(a, b);
}

inline dynamic_bitset& operator|=(dynamic_bitset& a, const dynamic_bitset& b)
{
    return ops_dynamic_bitset<std::bit_or<void>>(a, b);
}
