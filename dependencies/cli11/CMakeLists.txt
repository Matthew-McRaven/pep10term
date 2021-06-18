cmake_minimum_required(VERSION 3.10)

# set the project name
project(CLI11 VERSION 1.9.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library (CLI11 INTERFACE)
target_include_directories(CLI11 INTERFACE "${PROJECT_SOURCE_DIR}")