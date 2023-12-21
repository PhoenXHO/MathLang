cmake_minimum_required(VERSION 3.10)

project(MathLang VERSION 0.1)

# Configure a header file to pass some of the CMake settings to the source code
configure_file(mathlangconfig.h.in mathlangconfig.h)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Get a list of all source files in the 'src' directory
file(GLOB SOURCES "src/*.cpp")

# Add an executable target named MathLang with all source files in the 'src' directory
add_executable(mathlang "${SOURCES}")

# Include the 'include' directory for header files
target_include_directories(mathlang PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	)