cmake_minimum_required(VERSION 2.8.7)
project(autom8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(src)

#add_subdirectory(CoaPP)

# Enable the testsuite with
#   git clone https://github.com/google/googletest.git gtest
#IF(EXISTS "../gtest/" )
#    enable_testing()
#    add_subdirectory(tests)
#ENDIF()
