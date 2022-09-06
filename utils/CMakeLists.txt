cmake_minimum_required(VERSION 3.0.0)

include(CTest)
enable_testing()

add_library(utils
    commonfuncs.h
    commonfuncs.cpp)

target_compile_definitions(utils PRIVATE ${HOST_DEFINES})
target_compile_options(utils PRIVATE ${HOST_COMPILE_OPTIONS})
target_include_directories(utils 
    PUBLIC 
        .
    PRIVATE 
        ${HOST_INCLUDES})
set_property(TARGET utils PROPERTY COMPILE_PDB_NAME utils)
