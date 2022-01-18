cmake_minimum_required(VERSION 3.18)

add_executable(co_routine2 
coroutine.cpp)
#set_target_properties(co_routine2 PROPERTIES CXX_STANDARD 20)
target_include_directories(co_routine2 PUBLIC C:/Dev/Projects/cppcoro/include)
target_compile_options(co_routine2 PUBLIC  /await)
target_link_directories(co_routine2 PUBLIC C:/Dev/Projects/cppcoro/build/windows_x64_msvc15_debug/lib)
target_link_libraries(co_routine2 PUBLIC cppcoro WS2_32 synchronization)