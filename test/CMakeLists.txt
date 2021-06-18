cmake_minimum_required(VERSION 3.10)

# set the project name
project(TestTerm VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Allow version header to be included
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "**/*.cpp" "*.cpp")
add_executable (TestTerm ${sources})

target_link_libraries(TestTerm catch RTL-Lib LibTerm builtins) 

add_test (NAME TestTerm COMMAND TestTerm)