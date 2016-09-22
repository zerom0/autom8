cmake_minimum_required(VERSION 2.8.7)
project(autom8)

set(SRCS
        ResourceTests.cpp
        ../src/Resources/Resource.cpp
        ../src/Resources/Property.cpp)

include_directories(
        ../src
        ../src/Resources
        ../CoaPP/coap/include
        ../CoaPP/gtest/googletest/include
)

add_executable(autom8_testsuite ${SRCS})
#target_include_directories(testsuite PUBLIC include src)
target_link_libraries(autom8_testsuite gtest_main)

add_test(NAME autom8_testsuite COMMAND autom8_testsuite)
