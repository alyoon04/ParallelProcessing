#!/bin/bash
# Run benchmarks inside Docker container

set -e

echo "Running Benchmarks in Docker..."
echo "==============================="
echo ""

docker run -it --rm \
    -v "$(pwd)":/workspace \
    -w /workspace \
    cpp-parallel:latest \
    bash -c "
        mkdir -p build && cd build && \
        cmake .. && \
        make && \
        cd .. && \
        ./build/parallel_benchmark && \
        echo '' && \
        echo 'Running Python benchmark script...' && \
        python3 python/run_benchmark.py
    "

echo ""
echo "Benchmarks complete! Results saved in ./results/"
