cmake_minimum_required(VERSION 3.0.0)
project(idlparser VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(ast_parser
coreclasses.h
coreclasses.cpp
libraryLoader.cpp)

target_include_directories(ast_parser PUBLIC .)
target_link_libraries(ast_parser PUBLIC utils)
set_property(TARGET ast_parser PROPERTY COMPILE_PDB_NAME ast_parser)