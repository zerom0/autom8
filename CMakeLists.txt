cmake_minimum_required(VERSION 2.8.7)
project(autom8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(SYSTEM /usr/local/include)
link_directories(/usr/local/lib)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
