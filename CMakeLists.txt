cmake_minimum_required(VERSION 2.8)
project(gta3sc)

if(NOT CMAKE_COMPILER_IS_GNUXX AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_COMPILER_IS_GNUXX 1)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()

if(CMAKE_COMPILER_IS_GNUXX OR CMAKE_COMPILER_IS_CLANGXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
endif()

if(CMAKE_COMPILER_IS_GNUXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-placement-new")
endif()

if(CMAKE_COMPILER_IS_CLANGXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses-equality -Wno-tautological-compare")
endif()

include_directories(
  deps
  deps/rapidxml
  deps/cppformat
  deps/optional/include
  deps/expected/include
  deps/any/include
  deps/variant/include
  deps/libantlr3c-3.4/include
  deps/libantlr3c-3.4/
)

add_library(cppformat
  deps/cppformat/cppformat/format.cc
)

add_library(antlr3
  deps/libantlr3c-3.4/src/antlr3baserecognizer.c
  deps/libantlr3c-3.4/src/antlr3basetreeadaptor.c
  deps/libantlr3c-3.4/src/antlr3basetree.c
  deps/libantlr3c-3.4/src/antlr3bitset.c
  deps/libantlr3c-3.4/src/antlr3collections.c
  deps/libantlr3c-3.4/src/antlr3commontoken.c
  deps/libantlr3c-3.4/src/antlr3commontreeadaptor.c
  deps/libantlr3c-3.4/src/antlr3commontree.c
  deps/libantlr3c-3.4/src/antlr3commontreenodestream.c
  deps/libantlr3c-3.4/src/antlr3convertutf.c
  deps/libantlr3c-3.4/src/antlr3cyclicdfa.c
  #deps/libantlr3c-3.4/src/antlr3debughandlers.c
  deps/libantlr3c-3.4/src/antlr3encodings.c
  deps/libantlr3c-3.4/src/antlr3exception.c
  deps/libantlr3c-3.4/src/antlr3filestream.c
  deps/libantlr3c-3.4/src/antlr3inputstream.c
  deps/libantlr3c-3.4/src/antlr3intstream.c
  deps/libantlr3c-3.4/src/antlr3lexer.c
  deps/libantlr3c-3.4/src/antlr3parser.c
  deps/libantlr3c-3.4/src/antlr3rewritestreams.c
  deps/libantlr3c-3.4/src/antlr3string.c
  deps/libantlr3c-3.4/src/antlr3tokenstream.c
  deps/libantlr3c-3.4/src/antlr3treeparser.c
)

set(GTA3SC_SRC_AUTOGEN
  src/grammar/autogen/gta3scriptLexer.c
  src/grammar/autogen/gta3scriptParser.c
)

set(GTA3SC_SRC_MAIN
  src/annotation.hpp
  src/codegen.hpp
  src/commands.cpp
  src/commands.hpp
  src/compiler.hpp
  src/decompiler.hpp
  src/disassembler.hpp
  src/error.hpp
  src/main.cpp
  src/parser_lexer.cpp
  src/parser_syntax.cpp
  src/parser.hpp
  src/program.cpp
  src/program.hpp
  src/stdinc.h
  src/symtable.cpp
  src/symtable.hpp
  src/system.cpp
  src/system.hpp
)

set(GTA3SC_SRC_MISC
  src/cpp/any.hpp
  src/cpp/argv.hpp
  src/cpp/contracts.hpp
  src/cpp/file.hpp
  src/cpp/filesystem.hpp
  src/cpp/icompare.hpp
  src/cpp/optional.hpp
  src/cpp/scope_guard.hpp
  src/cpp/variant.hpp
  src/cpp/string_view.hpp
)

add_executable(gta3sc ${GTA3SC_SRC_MISC} ${GTA3SC_SRC_AUTOGEN} ${GTA3SC_SRC_MAIN})
source_group("autogen" FILES ${GTA3SC_SRC_AUTOGEN})
source_group("cpp" FILES ${GTA3SC_SRC_MISC})
source_group("" FILES ${GTA3SC_SRC_MAIN})

target_link_libraries(gta3sc cppformat antlr3)

if(CMAKE_COMPILER_IS_GNUXX OR CMAKE_COMPILER_IS_CLANGXX)
  target_link_libraries(gta3sc stdc++fs)
endif()

add_custom_command(TARGET gta3sc POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/config $<TARGET_FILE_DIR:gta3sc>/config)

#install(TARGETS gta3sc RUNTIME DESTINATION bin)
