cmake_minimum_required(VERSION 2.8.7)
project(autom8)

set(SRCS
        main.cpp
        Resources/And.cpp
        Resources/Input.cpp
        Resources/Not.cpp
        Resources/Or.cpp
        Resources/Property.cpp
        Resources/Resource.cpp
        Resources/Resources.cpp)

include_directories(../CoaPP/coap/include)

add_executable(autom8 ${SRCS})

target_link_libraries(autom8 pthread coap)
