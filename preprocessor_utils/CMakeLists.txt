cmake_minimum_required(VERSION 3.0.0)
project(idlparser VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(preprocessor_utils
macrohandler.h
macrohandler.cpp
edl_macro_handler.h
edl_macro_handler.cpp)

target_link_libraries(preprocessor_utils PUBLIC utils)
target_include_directories(preprocessor_utils PUBLIC .)
set_property(TARGET preprocessor_utils PROPERTY COMPILE_PDB_NAME preprocessor_utils)