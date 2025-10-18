# Build Instructions

Detailed build instructions for the Parallel Data Processing Library.

## Table of Contents
- [Quick Build](#quick-build)
- [Platform-Specific Instructions](#platform-specific-instructions)
- [Build Options](#build-options)
- [Docker Build](#docker-build)
- [Troubleshooting](#troubleshooting)

## Quick Build

```bash
# Clone or navigate to project directory
cd ParallelProcessing

# Create build directory
mkdir -p build
cd build

# Configure
cmake ..

# Build
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS

# Test
./test_suite

# Run example
./example
```

## Platform-Specific Instructions

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake git

# Install Python dependencies (optional, for benchmarking)
sudo apt-get install -y python3 python3-pip
pip3 install -r python/requirements.txt

# Build
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

### Linux (Fedora/RHEL)

```bash
# Install dependencies
sudo dnf install -y gcc gcc-c++ cmake git

# Python dependencies
sudo dnf install -y python3 python3-pip
pip3 install -r python/requirements.txt

# Build
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake

# Python dependencies
pip3 install -r python/requirements.txt

# Build
mkdir -p build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

### Windows (Visual Studio)

```powershell
# Install Visual Studio 2017 or later with C++ support
# Install CMake from https://cmake.org/download/

# Build using CMake GUI or command line
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release

# Or use Visual Studio to open the generated solution
```

### Windows (MinGW)

```bash
# Install MinGW-w64
# Install CMake

# Build
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make -j4
```

## Build Options

### Compiler Selection

```bash
# Use GCC
export CC=gcc
export CXX=g++
cmake ..

# Use Clang
export CC=clang
export CXX=clang++
cmake ..
```

### Build Types

```bash
# Debug build (default)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build (optimized)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Release with debug info
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Minimum size release
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
```

### Custom Compiler Flags

```bash
# Add custom flags
cmake .. -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native"

# Disable warnings
cmake .. -DCMAKE_CXX_FLAGS="-w"

# Enable all warnings
cmake .. -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic"
```

### Installation

```bash
# Build with custom install prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local

# Build and install
make
sudo make install

# This installs:
# - Headers to /usr/local/include/
# - Library to /usr/local/lib/
```

## Docker Build

### Basic Docker Build

```bash
# Build Docker image
./docker/build.sh

# Or manually
docker build -t cpp-parallel:latest .
```

### Run in Docker

```bash
# Interactive shell
./docker/run.sh

# Run specific command
docker run -it --rm -v $(pwd):/workspace cpp-parallel bash -c "
    mkdir -p build && cd build && \
    cmake .. && \
    make && \
    ./test_suite
"
```

### Docker with Different Compilers

```bash
# Use Clang in Docker
docker run -it --rm -v $(pwd):/workspace cpp-parallel bash -c "
    export CXX=clang++ && export CC=clang && \
    mkdir -p build && cd build && \
    cmake .. && \
    make
"
```

## Building Individual Targets

```bash
# Build only the library
make parallel_processing

# Build only tests
make test_suite

# Build only examples
make example

# Build only benchmarks
make parallel_benchmark

# Clean build
make clean

# Rebuild everything
make clean && make -j$(nproc)
```

## Verification

### Run Tests

```bash
cd build
./test_suite
```

Expected output:
```
Running Parallel Processing Library Tests
==========================================
Testing Map-Reduce... PASSED
Testing Parallel Sort... PASSED
...
All tests PASSED!
```

### Run Example

```bash
cd build
./example
```

### Run Benchmark

```bash
cd build
./parallel_benchmark 1 2 4 8
```

## Advanced Build Options

### Static vs Shared Library

The default build creates a static library. To build a shared library:

```cmake
# Modify CMakeLists.txt
add_library(parallel_processing SHARED ${LIBRARY_SOURCES})
```

### Cross-Compilation

```bash
# Example: Cross-compile for ARM
cmake .. \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=arm \
    -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
    -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++
```

### Sanitizers (Debug)

```bash
# Address sanitizer
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address -g"

# Thread sanitizer
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=thread -g"

# Undefined behavior sanitizer
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=undefined -g"
```

## Troubleshooting

### CMake Version Too Old

```bash
# Install newer CMake
pip3 install cmake --upgrade

# Or download from https://cmake.org/download/
```

### Compiler Not Found

```bash
# Install GCC
sudo apt-get install g++  # Debian/Ubuntu
sudo dnf install gcc-c++  # Fedora

# Or specify full path
cmake .. -DCMAKE_CXX_COMPILER=/usr/bin/g++-9
```

### Thread Library Not Found

```bash
# Install pthread development package
sudo apt-get install libpthread-stubs0-dev
```

### Python Dependencies

```bash
# Install Python packages
pip3 install pandas matplotlib numpy

# Or from requirements
pip3 install -r python/requirements.txt
```

### Permission Denied

```bash
# Make scripts executable
chmod +x docker/*.sh python/*.py

# Run with sudo if needed
sudo make install
```

### Out of Memory

```bash
# Reduce parallel jobs
make -j2  # Use only 2 jobs

# Or build serially
make
```

## Performance Build

For maximum performance:

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native -flto"

make -j$(nproc)
```

Flags explained:
- `-O3`: Aggressive optimization
- `-march=native`: Optimize for current CPU
- `-mtune=native`: Tune for current CPU
- `-flto`: Link-time optimization

## Minimal Build

For minimal binary size:

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DCMAKE_CXX_FLAGS="-Os -s"

make -j$(nproc)
strip build/parallel_benchmark  # Further reduce size
```

## Clean Build

```bash
# Clean build directory
cd build
make clean

# Or remove and recreate
cd ..
rm -rf build
mkdir build && cd build
cmake ..
make
```
