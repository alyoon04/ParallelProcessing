# Convenience Makefile for Parallel Processing Library
# This wraps CMake commands for easier usage

.PHONY: all build clean test run-tests run-example benchmark help install docker-build docker-run \
        test-asan test-tsan test-ubsan test-sanitizers build-asan build-tsan build-ubsan

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

# =========================================
# Sanitizer Targets
# =========================================

# Build with AddressSanitizer (memory errors, leaks)
build-asan:
	@echo "Building with AddressSanitizer..."
	@mkdir -p build-asan
	@cd build-asan && cmake .. -DENABLE_ASAN=ON && $(MAKE) -j$$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
	@echo "ASan build complete!"

# Build with ThreadSanitizer (data races)
build-tsan:
	@echo "Building with ThreadSanitizer..."
	@mkdir -p build-tsan
	@cd build-tsan && cmake .. -DENABLE_TSAN=ON && $(MAKE) -j$$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
	@echo "TSan build complete!"

# Build with UndefinedBehaviorSanitizer
build-ubsan:
	@echo "Building with UndefinedBehaviorSanitizer..."
	@mkdir -p build-ubsan
	@cd build-ubsan && cmake .. -DENABLE_UBSAN=ON && $(MAKE) -j$$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
	@echo "UBSan build complete!"

# Run tests with AddressSanitizer
test-asan: build-asan
	@echo "Running tests with AddressSanitizer..."
	@ASAN_OPTIONS=detect_leaks=1:abort_on_error=1 ./build-asan/test_suite

# Run tests with ThreadSanitizer
test-tsan: build-tsan
	@echo "Running tests with ThreadSanitizer..."
	@TSAN_OPTIONS=abort_on_error=1 ./build-tsan/test_suite

# Run tests with UndefinedBehaviorSanitizer
test-ubsan: build-ubsan
	@echo "Running tests with UndefinedBehaviorSanitizer..."
	@UBSAN_OPTIONS=print_stacktrace=1:abort_on_error=1 ./build-ubsan/test_suite

# Run all sanitizer tests (ASan first, then TSan, then UBSan)
test-sanitizers: test-asan test-tsan test-ubsan
	@echo "All sanitizer tests complete!"

# Clean sanitizer build directories
clean-sanitizers:
	@echo "Cleaning sanitizer build directories..."
	@rm -rf build-asan build-tsan build-ubsan
	@echo "Sanitizer builds cleaned!"

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
	@echo "Sanitizer Targets (Memory/Thread Safety):"
	@echo "  make build-asan       - Build with AddressSanitizer"
	@echo "  make build-tsan       - Build with ThreadSanitizer"
	@echo "  make build-ubsan      - Build with UndefinedBehaviorSanitizer"
	@echo "  make test-asan        - Run tests with AddressSanitizer"
	@echo "  make test-tsan        - Run tests with ThreadSanitizer"
	@echo "  make test-ubsan       - Run tests with UndefinedBehaviorSanitizer"
	@echo "  make test-sanitizers  - Run all sanitizer tests"
	@echo "  make clean-sanitizers - Clean sanitizer build directories"
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
