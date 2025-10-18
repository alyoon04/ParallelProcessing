#!/bin/bash
# Build script for Docker container

set -e

echo "Building Parallel Processing Library Docker Image..."
echo "===================================================="

# Build the Docker image
docker build -t cpp-parallel:latest .

echo ""
echo "Build complete!"
echo ""
echo "To run the container:"
echo "  docker run -it --rm -v \$(pwd):/workspace cpp-parallel"
echo ""
echo "Or use the run script:"
echo "  ./docker/run.sh"
