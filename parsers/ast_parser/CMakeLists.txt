cmake_minimum_required(VERSION 3.0.0)

include(CTest)
enable_testing()

add_library(ast_parser
    coreclasses.h
    coreclasses.cpp
    cpp_parser.h
    cpp_parser.cpp
    libraryLoader.cpp)

target_compile_definitions(ast_parser PRIVATE ${HOST_DEFINES})
target_compile_options(ast_parser PRIVATE ${HOST_COMPILE_OPTIONS})
target_include_directories(ast_parser 
    PUBLIC 
        .  
        utils
    PRIVATE 
        ${HOST_INCLUDES})

target_link_libraries(ast_parser 
    PUBLIC 
        utils
    PRIVATE
        ${HOST_LIBRARIES}
)
target_link_options(ast_parser PRIVATE ${HOST_LINK_OPTIONS})
target_link_directories(ast_parser PUBLIC ${SGX_LIBRARY_PATH})

set_property(TARGET ast_parser PROPERTY COMPILE_PDB_NAME ast_parser)
