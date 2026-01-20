cmake_minimum_required(VERSION 3.14)
project(ParallelProcessing VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sanitizer options
option(ENABLE_ASAN "Enable AddressSanitizer (memory error detection)" OFF)
option(ENABLE_LSAN "Enable LeakSanitizer (memory leak detection)" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer (data race detection)" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

# Sanitizer configuration
# Note: ASan and TSan cannot be used together
if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    # ASan includes LSan by default on Linux
endif()

if(ENABLE_LSAN AND NOT ENABLE_ASAN)
    message(STATUS "LeakSanitizer enabled (standalone)")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=leak")
endif()

if(ENABLE_TSAN)
    if(ENABLE_ASAN)
        message(FATAL_ERROR "ThreadSanitizer cannot be used with AddressSanitizer")
    endif()
    message(STATUS "ThreadSanitizer enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
endif()

if(ENABLE_UBSAN)
    message(STATUS "UndefinedBehaviorSanitizer enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()

# When any sanitizer is enabled, reduce optimization for better stack traces
if(ENABLE_ASAN OR ENABLE_LSAN OR ENABLE_TSAN OR ENABLE_UBSAN)
    string(REPLACE "-O3" "-O1" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

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

# Print sanitizer status summary
message(STATUS "Sanitizer configuration:")
message(STATUS "  AddressSanitizer (ENABLE_ASAN): ${ENABLE_ASAN}")
message(STATUS "  LeakSanitizer (ENABLE_LSAN): ${ENABLE_LSAN}")
message(STATUS "  ThreadSanitizer (ENABLE_TSAN): ${ENABLE_TSAN}")
message(STATUS "  UndefinedBehaviorSanitizer (ENABLE_UBSAN): ${ENABLE_UBSAN}")
