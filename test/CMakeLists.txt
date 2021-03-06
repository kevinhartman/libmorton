cmake_minimum_required(VERSION 3.10)
project(test)

option(ENABLE_BMI2 "Enable BMI2." ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")

if(ENABLE_BMI2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mbmi2")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconstexpr-steps=2147483647")
endif()

include(CTest)

add_executable(libmorton_test
        libmorton_test.cpp
        libmorton_test.h
        libmorton_test_2D.h
        libmorton_test_3D.h
        timer.h
        util.h)

add_subdirectory(morton-nd)
target_link_libraries(libmorton_test PRIVATE morton-nd::MortonND)

add_test(NAME libmorton_test COMMAND libmorton_test)