# Dockerfile for Parallel Processing Library
# Supports building and testing on Linux with GCC and Clang

FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    clang \
    python3 \
    python3-pip \
    git \
    wget \
    vim \
    && rm -rf /var/lib/apt/lists/*

# Upgrade pip and install Python dependencies
RUN pip3 install --upgrade pip

# Copy Python requirements and install
COPY python/requirements.txt /tmp/requirements.txt
RUN pip3 install -r /tmp/requirements.txt

# Set up working directory
WORKDIR /workspace

# Copy project files
COPY . /workspace/

# Set environment variables
ENV CXX=g++
ENV CC=gcc

# Default command: show instructions
CMD ["bash", "-c", "echo 'Parallel Processing Library Docker Container'; \
     echo '=========================================='; \
     echo ''; \
     echo 'To build the project:'; \
     echo '  mkdir -p build && cd build'; \
     echo '  cmake ..'; \
     echo '  make'; \
     echo ''; \
     echo 'To run tests:'; \
     echo '  ./build/test_suite'; \
     echo ''; \
     echo 'To run examples:'; \
     echo '  ./build/example'; \
     echo ''; \
     echo 'To run benchmarks:'; \
     echo '  ./build/parallel_benchmark'; \
     echo '  python3 python/run_benchmark.py'; \
     echo ''; \
     echo 'To use Clang instead of GCC:'; \
     echo '  export CXX=clang++'; \
     echo '  export CC=clang'; \
     echo ''; \
     exec bash"]
