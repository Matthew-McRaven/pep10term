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
# Integration tests.
# Correctness of output will be checked in other tests.
# Test that we can assemble a figure without crashing.
add_test(NAME PepTerm COMMAND /../src/PepTerm asm -src://pep10.05.19.pep -o "fig0519.pepo")