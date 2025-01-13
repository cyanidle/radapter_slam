cmake_minimum_required(VERSION 3.16)

project(radapter_slam VERSION 0.0.1 LANGUAGES C CXX ASM)

add_subdirectory(radapter)

find_package(Qt5 REQUIRED COMPONENTS Core)

set(CMAKE_AUTOMOC ON)

create_radapter_plugin(radapter_slam
    FILES src/radapter_slam.cpp
)
