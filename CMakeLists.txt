cmake_minimum_required(VERSION 3.14)

# Set the project name and version
project(HelloWorld VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Collect all source files from src directory
file(GLOB SOURCES "src/*.cpp")

# Add the executable
add_executable(filter ${SOURCES})
set_target_properties(filter PROPERTIES OUTPUT_NAME "filter")

# --- GoogleTest (for integration tests that run the built binary) ---
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Test that runs the built binary. The test source will launch the binary by path.
add_executable(test_filter tests/test_filter.cpp)
target_link_libraries(test_filter PRIVATE GTest::gtest)

# Provide the path to the built executable to the test via a compile definition.
if (WIN32)
	# Visual Studio places executables in configuration subfolders (Debug/Release)
	set(EXE_SUBDIR $<CONFIG>)
else()
	set(EXE_SUBDIR "")
endif()

add_test(NAME run_filter_binary COMMAND test_filter $<TARGET_FILE:filter>)
set_tests_properties(run_filter_binary PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
