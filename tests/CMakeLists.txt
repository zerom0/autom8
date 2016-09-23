cmake_minimum_required(VERSION 2.8.7)
project(autom8)

set(SRCS
        ResourceTests.cpp
        ../src/Resources/Resource.cpp
        ../src/Resources/Property.cpp)

include_directories(
        ../src
        ../src/Resources
)

add_executable(testsuite ${SRCS})

target_link_libraries(testsuite gtest_main)

add_test(NAME testsuite COMMAND testsuite)
