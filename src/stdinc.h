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
#include "cpp/string_view.hpp"
#include "cpp/icompare.hpp"
#include "cpp/contracts.hpp"
#include "cpp/file.hpp"

// TODO make all maps and such be transparently comparable
// TODO transform lots of structs into class; make members private;

using std::shared_ptr;
using std::weak_ptr;
using dynamic_bitset = std::vector<bool>;

class SyntaxTree;
class ProgramContext;
struct Commands;
struct Command;
struct Var;
struct Script;
struct Scope;
struct SymTable;
struct CodeGenerator;
struct CodeGeneratorBase;

using EntityType = uint16_t;


#ifndef _MSC_VER
#   define __debugbreak()
#endif

