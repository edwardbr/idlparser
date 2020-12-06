cmake_minimum_required(VERSION 3.0.0)

set(FLATBUFFERS_FLATC_EXECUTABLE "C:/.hunter/_Base/36e7f44/a91fcca/0ce5228/Install/bin/flatc.exe")

include(Findflatbuffers)

build_flatbuffers(
    "C:/Dev/experiments/idlparser/build/preproc/logger_test_enclave_destroy_request.fbs"
    "."
    flatbuffers_1 
    ""
    "."
    ""
    "")              

add_library(flatbuffer_generated INTERFACE)  

message(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_dependencies(flatbuffer_generated flatbuffers_1 edl_circular_buffer_generator)
