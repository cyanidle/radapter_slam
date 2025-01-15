cmake_minimum_required(VERSION 3.16)

project(radapter_slam VERSION 0.0.1 LANGUAGES C CXX ASM)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(radapter)

find_package(PkgConfig REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Boost REQUIRED system serialization filesystem thread)
find_package(Eigen3 REQUIRED)
find_package(TBB REQUIRED)
find_package(Ceres REQUIRED COMPONENTS SuiteSparse)

set(CMAKE_AUTOMOC ON)

create_radapter_plugin(radapter_slam
    FILES src/radapter_slam.cpp
)

set(karto_dir 3rdparty/karto_sdk/)
set(slam_dir 3rdparty/slam_toolbox/)

###

add_library(kartoSlamToolbox STATIC
    ${karto_dir}/src/Karto.cpp
    ${karto_dir}/src/Mapper.cpp)

target_include_directories(kartoSlamToolbox PUBLIC
    ${karto_dir}/include
)

target_link_libraries(kartoSlamToolbox PUBLIC
    Eigen3::Eigen
    Boost::boost
    TBB::tbb
)

###

add_library(ceres_solver_plugin STATIC
    ${slam_dir}/solvers/ceres_solver.cpp)
target_include_directories(ceres_solver_plugin PUBLIC ${slam_dir})
target_link_libraries(ceres_solver_plugin PUBLIC
    Ceres::ceres
    kartoSlamToolbox
    Boost::boost
)

###

add_library(toolbox_common STATIC
    ${slam_dir}/src/slam_toolbox_common.cpp
    ${slam_dir}/src/map_saver.cpp
    ${slam_dir}/src/loop_closure_assistant.cpp
    ${slam_dir}/src/laser_utils.cpp
    ${slam_dir}/src/slam_mapper.cpp)


target_link_libraries(toolbox_common PUBLIC
    kartoSlamToolbox
    Boost::boost)

target_link_libraries(radapter_slam PRIVATE
    toolbox_common
    ceres_solver_plugin
)

target_sources(radapter_slam PRIVATE
    ${slam_dir}/src/slam_toolbox_async.cpp)
