cmake_minimum_required(VERSION 3.0.0)
project(idlparser VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(macro_parser
macro_parser.h
macro_parser.cpp)

target_link_libraries(macro_parser PUBLIC utils)
target_include_directories(macro_parser PUBLIC .)
set_property(TARGET macro_parser PROPERTY COMPILE_PDB_NAME macro_parser)