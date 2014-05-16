project (Soccer)
cmake_minimum_required (VERSION 2.8)
set (CMAKE_CXX_FLAGS "-Wall -std=c++11 -O2")

include_directories (${Soccer_SOURCE_DIR}/src)
set (EXECUTABLE_OUTPUT_PATH "${Soccer_BINARY_DIR}/bin")
set (LIBRARY_OUTPUT_PATH "${Soccer_BINARY_DIR}/lib")

add_subdirectory (src)

install (DIRECTORY data DESTINATION "${Soccer_BINARY_DIR}")
