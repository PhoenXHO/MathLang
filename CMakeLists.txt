cmake_minimum_required(VERSION 3.10)

project(MathLang VERSION 0.1)

# Configure a header file to pass some of the CMake settings to the source code
configure_file(mathlangconfig.h.in mathlangconfig.h)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Get a list of all source files in the 'src' directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add an executable target named MathLang with all source files in the 'src' directory
add_executable(mathlang "${SOURCES}")

# Add all header files to the include directories for the target
file(GLOB_RECURSE HEADER_DIRS LIST_DIRECTORIES true "include")


# Include the 'include' directory for header files
target_include_directories(mathlang PUBLIC
	"${PROJECT_BINARY_DIR}"
	#"${HEADER_DIRS}"
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/ast"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/compiling"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/mathobj"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/operator"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/parsing"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/utils"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/vm"
	)
