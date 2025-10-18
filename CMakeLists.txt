cmake_minimum_required(VERSION 3.14)
project(ParallelProcessing VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

# Find Threads package
find_package(Threads REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Library source files
set(LIBRARY_SOURCES
    src/thread_manager.cpp
    src/map_reduce.cpp
    src/parallel_sort.cpp
    src/parallel_aggregation.cpp
)

# Create the library
add_library(parallel_processing STATIC ${LIBRARY_SOURCES})
target_link_libraries(parallel_processing PRIVATE Threads::Threads)

# Example executable
add_executable(example examples/example.cpp)
target_link_libraries(example PRIVATE parallel_processing Threads::Threads)

# Benchmark executable
add_executable(parallel_benchmark benchmarks/benchmark.cpp)
target_link_libraries(parallel_benchmark PRIVATE parallel_processing Threads::Threads)

# Test executable
add_executable(test_suite tests/test_all.cpp)
target_link_libraries(test_suite PRIVATE parallel_processing Threads::Threads)

# Install targets
install(TARGETS parallel_processing
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
