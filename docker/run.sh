#!/bin/bash
# Run script for Docker container

set -e

# Check if image exists
if ! docker image inspect cpp-parallel:latest > /dev/null 2>&1; then
    echo "Docker image 'cpp-parallel:latest' not found."
    echo "Building image first..."
    ./docker/build.sh
fi

echo "Starting Parallel Processing Library Container..."
echo "================================================="
echo ""

# Run the container with current directory mounted
docker run -it --rm \
    -v "$(pwd)":/workspace \
    -w /workspace \
    cpp-parallel:latest
