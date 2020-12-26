cmake_minimum_required(VERSION 3.0.0)
project(idlparser VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(edl_circular_buffer_generator 
main.cpp
flatbuffer_gen.cpp
flatbuffer_gen.h
ocall_gen.cpp
ocall_gen.h)

target_link_libraries(edl_circular_buffer_generator 
    PUBLIC 
        ast_parser 
        preprocessor_utils 
        generators)
set_property(TARGET edl_circular_buffer_generator PROPERTY COMPILE_PDB_NAME edl_circular_buffer_generator)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
