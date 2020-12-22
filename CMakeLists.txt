#cmake_minimum_required(VERSION 3.17)
#project(qcutils C)
include_directories(include)
add_subdirectory(src)
add_subdirectory(test)

set(CMAKE_C_STANDARD 99)

