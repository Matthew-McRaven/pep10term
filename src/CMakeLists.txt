cmake_minimum_required(VERSION 3.10)

# set the project name
project(PepTerm VERSION 10.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Run clang tidy iff it is installed and on the path.
find_program(CLANG_TIDY "clang-tidy")
# Emscripten seems to screw with clang-tidy's ability to work.
if(CLANG_TIDY AND  (NOT EMSCRIPTEN))
	set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,clang-analyzer-*)
endif()

find_package( PythonInterp 3.7 REQUIRED )
# Generate the registration code at configure time rather than build time
execute_process(COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/gen.py")

configure_file(term_version.hpp.in term_version.hpp)
find_package(Git)
# the commit's SHA1, and whether the building workspace was dirty or not
execute_process(COMMAND
  "${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_SHA1
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# the date of the commit
execute_process(COMMAND
  "${GIT_EXECUTABLE}" log -1 --format=%ad --date=local
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_DATE
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# the subject of the commit
execute_process(COMMAND
  "${GIT_EXECUTABLE}" log -1 --format=%s
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_COMMIT_SUBJECT
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# generate term_version.hpp
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/term_version.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/term_version.hpp" @ONLY)

list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/version.cc" version.hh)

# Gather all files except for main, and treat those as the library powering PepTerm.
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "**/*.cpp" "*.cpp")
# Filter instead of remove since I don't know the path to main.
# See: https://stackoverflow.com/a/49377205
list(FILTER sources EXCLUDE REGEX ".*main.cpp$")

# Build a library containing all code not related to the TUI.
add_library(LibTerm STATIC ${sources})
target_include_directories(LibTerm PUBLIC ${Boost_INCLUDE_DIR} CLI11)
target_link_libraries(LibTerm PUBLIC ${Boost_LIBRARIES} CLI11 RTL-Lib builtins)

# Add the executable, which contains the TUI.
add_executable(PepTerm "main.cpp")
target_include_directories(PepTerm PUBLIC ${Boost_INCLUDE_DIR} LibTerm CLI11)
target_link_libraries(PepTerm PUBLIC ${Boost_LIBRARIES} CLI11 RTL-Lib LibTerm builtins fmt)

# Allow version header to be included
target_include_directories(PepTerm PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(PepTerm PUBLIC "${PROJECT_SOURCE_DIR}")