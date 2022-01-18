cmake_minimum_required(VERSION 3.18)

add_executable(co_routine 
coroutine.cpp)
set_target_properties(co_routine PROPERTIES CXX_STANDARD 20)