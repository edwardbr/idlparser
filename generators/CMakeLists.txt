cmake_minimum_required(VERSION 3.0.0)
project(idlparser VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(generators
b_json.cpp
b_json.h
ClientClassWrapper.cpp
ClientClassWrapper.h  
CMakeLists.txt        
ComObjectGen.cpp      
ComObjectGen.h        
CPPParser.cpp
CPPParser.h
DumpFunction.cpp
DumpFunction.h
InterpretObject.h
javascript_json.cpp
javascript_json.h
nb_json.cpp
nb_json.h)

target_link_libraries(generators PUBLIC utils ast_parser)
target_include_directories(generators PUBLIC .)
set_property(TARGET generators PROPERTY COMPILE_PDB_NAME generators)