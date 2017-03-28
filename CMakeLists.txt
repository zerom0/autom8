cmake_minimum_required(VERSION 2.8.7)
project(autom8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_path(COAP_DIR coap)
find_library(COAP_LIB coap)

message(${COAP_DIR})
message(${COAP_LIB})

add_subdirectory(src)

# Locate GTest
find_package(GTest)

enable_testing()
add_subdirectory(tests)
