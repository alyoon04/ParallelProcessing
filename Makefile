# Convenience Makefile for Parallel Processing Library
# This wraps CMake commands for easier usage

.PHONY: all build clean test run-tests run-example benchmark help install docker-build docker-run

# Default target
all: build

# Build the project
build:
	@echo "Building Parallel Processing Library..."
	@mkdir -p build
	@cd build && cmake .. && $(MAKE) -j$$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
	@echo "Build complete!"

# Clean build artifacts
clean:
	@echo "Cleaning build directory..."
	@rm -rf build
	@echo "Clean complete!"

# Rebuild from scratch
rebuild: clean build

# Run test suite
test: build
	@echo "Running test suite..."
	@./build/test_suite

# Alias for test
run-tests: test

# Run example program
run-example: build
	@echo "Running example program..."
	@./build/example

# Run benchmark
benchmark: build
	@echo "Running benchmark..."
	@./build/parallel_benchmark 1 2 4 8

# Run Python benchmark automation
benchmark-python: build
	@echo "Running Python benchmark automation..."
	@python3 python/run_benchmark.py

# Generate plots from results
plot:
	@echo "Generating performance plots..."
	@pip3 install -r python/requirements.txt
	@python3 python/plot_benchmark.py results/benchmark_results.csv results/

# Full benchmark with plots
full-benchmark: build
	@echo "Running full benchmark suite..."
	@python3 python/run_benchmark.py
	@python3 python/plot_benchmark.py results/benchmark_results.csv results/
	@echo "Benchmark complete! Results in results/ directory"

# Install library system-wide (may require sudo)
install: build
	@echo "Installing library..."
	@cd build && $(MAKE) install

# Build Docker image
docker-build:
	@echo "Building Docker image..."
	@./docker/build.sh

# Run Docker container
docker-run:
	@echo "Running Docker container..."
	@./docker/run.sh

# Run Docker benchmark
docker-benchmark:
	@echo "Running Docker benchmark..."
	@./docker/benchmark.sh

# Setup Python environment
setup-python:
	@echo "Setting up Python environment..."
	@pip3 install -r python/requirements.txt

# Help target
help:
	@echo "Parallel Processing Library - Make Targets"
	@echo "==========================================="
	@echo ""
	@echo "Build Targets:"
	@echo "  make                  - Build the project (default)"
	@echo "  make build            - Build the project"
	@echo "  make clean            - Remove build artifacts"
	@echo "  make rebuild          - Clean and rebuild"
	@echo ""
	@echo "Test Targets:"
	@echo "  make test             - Run test suite"
	@echo "  make run-tests        - Alias for 'make test'"
	@echo "  make run-example      - Run example program"
	@echo ""
	@echo "Benchmark Targets:"
	@echo "  make benchmark        - Run C++ benchmark"
	@echo "  make benchmark-python - Run Python benchmark automation"
	@echo "  make plot             - Generate plots from results"
	@echo "  make full-benchmark   - Run benchmarks and generate plots"
	@echo ""
	@echo "Docker Targets:"
	@echo "  make docker-build     - Build Docker image"
	@echo "  make docker-run       - Run interactive Docker container"
	@echo "  make docker-benchmark - Run benchmarks in Docker"
	@echo ""
	@echo "Other Targets:"
	@echo "  make install          - Install library system-wide"
	@echo "  make setup-python     - Install Python dependencies"
	@echo "  make help             - Show this help message"
