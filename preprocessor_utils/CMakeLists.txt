cmake_minimum_required(VERSION 3.0.0)
project(idlparser VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(preprocessor_utils
macrohandler.h
macrohandler.cpp)

target_link_libraries(preprocessor_utils PUBLIC utils)
target_include_directories(preprocessor_utils PUBLIC .)