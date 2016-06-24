cmake_minimum_required(VERSION 2.8)
project(gta3sc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-placement-new")

include_directories(
  deps
  deps/rapidxml
  deps/cppformat
  deps/optional/include
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

add_executable(gta3sc
  src/grammar/autogen/gta3scriptLexer.c
  src/grammar/autogen/gta3scriptParser.c
  src/commands.cpp
  src/main.cpp
  src/parser.cpp
  src/symtable.cpp
  src/system.cpp
)

target_link_libraries(gta3sc cppformat antlr3 stdc++fs)

add_custom_command(TARGET gta3sc POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/config $<TARGET_FILE_DIR:gta3sc>/config)

#install(TARGETS gta3sc RUNTIME DESTINATION bin)
