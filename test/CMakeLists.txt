cmake_minimum_required(VERSION 3.10)
project(test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -mbmi2")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconstexpr-steps=2147483647")
endif()

add_executable(libmorton_test
        libmorton_test.cpp
        libmorton_test.h
        libmorton_test_2D.h
        libmorton_test_3D.h
        timer.h
        util.h)

add_subdirectory(morton-nd)
target_link_libraries(libmorton_test PRIVATE morton-nd::MortonND)