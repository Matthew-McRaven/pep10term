cmake_minimum_required(VERSION 3.10)

# Run clang tidy iff it is installed and on the path.
find_program(CLANG_TIDY "clang-tidy")
# Emscripten seems to screw with clang-tidy's ability to work.
if(CLANG_TIDY AND  (NOT EMSCRIPTEN))
	set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,clang-analyzer-*)
endif()

configure_file(version.hpp.in version.hpp)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "**/*.cpp" "*.cpp")
# add the executable
add_executable(PepTerm ${sources})

target_include_directories(PepTerm PUBLIC ${Boost_INCLUDE_DIR} )
target_link_libraries(PepTerm PUBLIC ${Boost_LIBRARIES} )

# Allow version header to be included
target_include_directories(PepTerm PUBLIC "${PROJECT_BINARY_DIR}/")
target_include_directories(PepTerm PUBLIC "${PROJECT_SOURCE_DIR}/")