cmake_minimum_required(VERSION 3.10)

# Run clang tidy iff it is installed and on the path.
find_program(CLANG_TIDY "clang-tidy")
# Emscripten seems to screw with clang-tidy's ability to work.
if(CLANG_TIDY AND  (NOT EMSCRIPTEN))
	set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,clang-analyzer-*)
endif()

configure_file(version.hpp.in version.hpp)

# Gather all files except for main, and treat those as the library powering PepTerm.
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "**/*.cpp" "*.cpp")
# Filter instead of remove since I don't know the path to main.
# See: https://stackoverflow.com/a/49377205
list(FILTER sources EXCLUDE REGEX ".*main.cpp$")

# Build a library containing all code not related to the TUI.
add_library(LibTerm STATIC ${sources})
target_include_directories(LibTerm PUBLIC ${Boost_INCLUDE_DIR} CLI11)
target_link_libraries(LibTerm PUBLIC ${Boost_LIBRARIES} CLI11 RTL-Lib)

# Add the executable, which contains the TUI.
add_executable(PepTerm "main.cpp")
target_include_directories(PepTerm PUBLIC ${Boost_INCLUDE_DIR} LibTerm CLI11)
target_link_libraries(PepTerm PUBLIC ${Boost_LIBRARIES} CLI11 RTL-Lib LibTerm builtins)

# Allow version header to be included
target_include_directories(PepTerm PUBLIC "${PROJECT_BINARY_DIR}/")
target_include_directories(PepTerm PUBLIC "${PROJECT_SOURCE_DIR}/")