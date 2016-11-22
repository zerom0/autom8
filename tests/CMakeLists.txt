cmake_minimum_required(VERSION 2.8.7)
project(autom8)

set(SRCS
        AndTests.cpp
        NotTests.cpp
        ResourceTests.cpp
        OrTests.cpp
        ../src/Resources/And.cpp
        ../src/Resources/Not.cpp
        ../src/Resources/Resource.cpp
        ../src/Resources/Property.cpp
        ../src/Resources/Or.cpp)

include_directories(
        ../src
        ../src/Resources
)

add_executable(testsuite ${SRCS})

target_link_libraries(testsuite gtest_main gtest)

add_test(NAME testsuite COMMAND testsuite)
