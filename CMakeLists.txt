# Dependencies
# - CMake 3.29.7
# - Boost 1.74.0
# - GMP
# - MPFR


cmake_minimum_required(VERSION 3.29.7)
message(STATUS "CMake version: ${CMAKE_VERSION}")

# Set the project name and version number
# Version format: <major>.<minor>.<patch>-<status>.<status-version>
project(MathLang LANGUAGES CXX)
set(MathLang_VERSION_MAJOR 0)
set(MathLang_VERSION_MINOR 2)
set(MathLang_VERSION_PATCH 0)

set(MathLang_PRE_RELEASE true)
set(MathLang_VERSION_STATUS "alpha")
set(MathLang_VERSION_STATUS_VERSION 1)

# Create a header file containing the version number
configure_file(mathlangconfig.hpp.in mathlangconfig.hpp)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find the Boost libraries
set(BOOST_COMPONENTS
	program_options
	#multiprecision
)
find_package(Boost 1.74.0 REQUIRED COMPONENTS ${BOOST_COMPONENTS})
if(Boost_FOUND)
	message(STATUS "Boost version: ${Boost_VERSION}")
	message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
	message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

	# Add the Boost include directories
	include_directories(${Boost_INCLUDE_DIRS})
else()
	message(FATAL_ERROR "Boost not found")
endif()


# Add the cmake directory to the module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
# Find GMP and MPFR
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)

if(GMP_FOUND)
	message(STATUS "GMP version: ${GMP_VERSION}")
	message(STATUS "GMP include dirs: ${GMP_INCLUDE_DIRS}")
	message(STATUS "GMP libraries: ${GMP_LIBRARIES}")

	# Add the GMP include directories
	include_directories(${GMP_INCLUDE_DIRS})
else()
	message(FATAL_ERROR "GMP not found")
endif()

if(MPFR_FOUND)
	message(STATUS "MPFR version: ${MPFR_VERSION}")
	message(STATUS "MPFR include dirs: ${MPFR_INCLUDE_DIRS}")
	message(STATUS "MPFR libraries: ${MPFR_LIBRARIES}")

	# Add the MPFR include directories
	include_directories(${MPFR_INCLUDE_DIRS})
else()
	message(FATAL_ERROR "MPFR not found")
endif()



# Add GoogleTest using FetchContent
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()



# Add the test executable target
file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")
add_executable(MathLangTest ${TEST_SOURCES})

# Link the GoogleTest libraries
target_link_libraries(MathLangTest gtest gtest_main)

# Discover and run the tests
include(GoogleTest)
gtest_discover_tests(MathLangTest)



# Add the executable target
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(MathLang ${SOURCES})
message(STATUS "Sources: ${SOURCES}")

# Link the Boost libraries
foreach(BOOST_COMPONENT ${BOOST_COMPONENTS})
	target_link_libraries(MathLang Boost::${BOOST_COMPONENT})	
endforeach()

# Link the GMP and MPFR libraries
target_link_libraries(MathLang ${GMP_LIBRARIES} ${MPFR_LIBRARIES})



target_include_directories(MathLang PRIVATE
	"${PROJECT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	#${HEADER_PATHS}
)

# Enable the use of response files for long command lines
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 1)