cmake_minimum_required(VERSION 3.0.0)
project(idlparser VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(utils
commonfuncs.h
commonfuncs.cpp
function_timer.h
function_timer.cpp)

target_include_directories(utils PUBLIC .)
set_property(TARGET utils PROPERTY COMPILE_PDB_NAME utils)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
