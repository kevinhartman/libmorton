cmake_minimum_required(VERSION 3.10)
project(test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconstexpr-steps=2147483647")

include_directories(.)

add_executable(libmorton_test
        libmorton_test.cpp
        libmorton_test.h
        libmorton_test_2D.h
        libmorton_test_3D.h
        libmorton_test_controlvalues.h
        timer.h
        util.h)
