cmake_minimum_required(VERSION 3.0.0)
project(idlparser VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(utils)
add_subdirectory(preprocessor_utils)
add_subdirectory(preprocessor)
add_subdirectory(ast_parser)
add_subdirectory(generators)
add_subdirectory(ajax_coroutine_generator)