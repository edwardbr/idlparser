cmake_minimum_required(VERSION 3.0.0)

#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
add_subdirectory(com_wrapper_generator)
add_subdirectory(edl_parser)
add_subdirectory(enclave_marshalling_generator)

#add_subdirectory(tests/fb_gen_test)
add_subdirectory(tests/flatbuffer)