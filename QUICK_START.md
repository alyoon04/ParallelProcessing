# Quick Start Guide

Get started with the Parallel Data Processing Library in 5 minutes!

## Prerequisites Check

Before building, ensure you have:

```bash
# Check for C++ compiler
g++ --version  # or clang++ --version

# Check for CMake
cmake --version

# Check for Python (optional, for benchmarking)
python3 --version
```

If any are missing, see [BUILD.md](BUILD.md) for installation instructions.

## 1. Build the Library

```bash
# Navigate to project directory
cd ParallelProcessing

# Create and enter build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make -j$(nproc)  # Linux
# or
make -j$(sysctl -n hw.ncpu)  # macOS
```

This creates:
- `libparallel_processing.a` - The library
- `test_suite` - Test executable
- `example` - Example program
- `parallel_benchmark` - Benchmark tool

## 2. Run Tests

Verify everything works:

```bash
./test_suite
```

You should see:
```
Running Parallel Processing Library Tests
==========================================
Testing Map-Reduce... PASSED
Testing Parallel Sort... PASSED
...
All tests PASSED!
```

## 3. Run the Example

See the library in action:

```bash
./example
```

Output shows examples of:
- Map-Reduce operations
- Parallel sorting
- Aggregation functions
- Custom reducers
- Async operations

## 4. Run Benchmarks

Compare performance across thread counts:

```bash
# C++ benchmark with 1, 2, 4, 8 threads
./parallel_benchmark 1 2 4 8

# Python automated benchmark (from project root)
cd ..
python3 python/run_benchmark.py
```

Results are saved to `results/benchmark_results.csv`

## 5. Visualize Results

Generate performance plots:

```bash
# Install Python dependencies (first time only)
pip3 install -r python/requirements.txt

# Generate plots
python3 python/plot_benchmark.py results/benchmark_results.csv results/
```

Plots are saved to `results/` directory:
- `speedup_comparison.png` - Overall speedup
- `efficiency.png` - Parallel efficiency
- Individual operation plots

## Using the Library

### Include in Your Project

```cpp
#include "parallel.h"

int main() {
    std::vector<int> data(1000000);
    std::iota(data.begin(), data.end(), 0);

    // Use 8 threads for computation
    auto sum = Parallel::parallelSum(data, 8);

    return 0;
}
```

### Compile Your Program

```bash
g++ -std=c++17 -O3 \
    -I/path/to/ParallelProcessing/include \
    -L/path/to/ParallelProcessing/build \
    -lparallel_processing -lpthread \
    your_program.cpp -o your_program
```

Or with CMake:

```cmake
find_library(PARALLEL_PROCESSING parallel_processing
             PATHS /path/to/ParallelProcessing/build)

add_executable(your_program your_program.cpp)
target_include_directories(your_program PRIVATE
    /path/to/ParallelProcessing/include)
target_link_libraries(your_program ${PARALLEL_PROCESSING} pthread)
```

## Docker Quick Start

If you prefer Docker:

```bash
# Build Docker image
./docker/build.sh

# Run interactive container
./docker/run.sh

# Inside container:
mkdir -p build && cd build
cmake ..
make
./test_suite
```

Or run benchmarks directly:

```bash
./docker/benchmark.sh
```

## Common Operations

### Map-Reduce

```cpp
auto map_fn = [](int x) { return x * x; };
auto reduce_fn = [](long long a, long long b) { return a + b; };
auto result = Parallel::mapReduce(data, map_fn, reduce_fn, 8);
```

### Parallel Sort

```cpp
Parallel::parallelSort(data, 8);

// With custom comparator
Parallel::parallelSort(data, std::greater<int>(), 8);
```

### Aggregation

```cpp
auto sum = Parallel::parallelSum(data, 8);
auto mean = Parallel::parallelMean(data, 8);
auto min_val = Parallel::parallelMin(data, 8);
auto max_val = Parallel::parallelMax(data, 8);
```

### Custom Reduce

```cpp
auto product_fn = [](int a, int b) { return a * b; };
auto product = Parallel::parallelReduce(data, product_fn, 1, 8);
```

### Async Operations

```cpp
auto future = Parallel::mapReduceAsync(data, map_fn, reduce_fn, 8);
// Do other work...
auto result = future.get();
```

## Performance Tips

1. **Thread Count**: Start with `std::thread::hardware_concurrency()` or pass `0` for auto-detection
2. **Data Size**: Library automatically uses serial processing for small datasets (< 1000 elements)
3. **Benchmarking**: Always benchmark with your specific workload and data patterns

## Next Steps

- Read the full [README.md](README.md) for detailed API documentation
- Check [BUILD.md](BUILD.md) for platform-specific build instructions
- Explore `examples/example.cpp` for more usage patterns
- Review `tests/test_all.cpp` to understand expected behavior

## Getting Help

If you encounter issues:

1. Check [BUILD.md](BUILD.md) troubleshooting section
2. Verify all tests pass with `./build/test_suite`
3. Ensure you're using C++17 or later
4. Check compiler and CMake versions meet requirements

## Summary

```bash
# Complete quick start
mkdir -p build && cd build
cmake ..
make -j$(nproc)
./test_suite
./example
./parallel_benchmark 1 2 4 8
cd ..
python3 python/run_benchmark.py
```

That's it! You're ready to use parallel processing in your applications.
