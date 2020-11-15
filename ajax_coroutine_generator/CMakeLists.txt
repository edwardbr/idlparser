cmake_minimum_required(VERSION 3.0.0)
project(idlparser VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(ajax_coroutine_generator 
main.cpp)

target_link_libraries(ajax_coroutine_generator PUBLIC ast_parser generators)
set_property(TARGET ajax_coroutine_generator PROPERTY COMPILE_PDB_NAME ajax_coroutine_generator)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
