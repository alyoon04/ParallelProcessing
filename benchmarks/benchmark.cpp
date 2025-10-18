#include "parallel.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <numeric>
#include <random>
#include <iomanip>
#include <string>
#include <sstream>

using namespace std::chrono;

struct BenchmarkResult {
    std::string operation;
    size_t num_threads;
    size_t data_size;
    double execution_time_ms;
    double speedup;
};

void printHeader() {
    std::cout << std::setw(20) << "Operation"
              << std::setw(12) << "Data Size"
              << std::setw(10) << "Threads"
              << std::setw(15) << "Time (ms)"
              << std::setw(12) << "Speedup"
              << std::endl;
    std::cout << std::string(69, '-') << std::endl;
}

void printResult(const BenchmarkResult& result) {
    std::cout << std::setw(20) << result.operation
              << std::setw(12) << result.data_size
              << std::setw(10) << result.num_threads
              << std::setw(15) << std::fixed << std::setprecision(3) << result.execution_time_ms
              << std::setw(12) << std::fixed << std::setprecision(2) << result.speedup
              << std::endl;
}

template<typename Func>
double benchmarkOperation(Func operation, size_t iterations = 1) {
    auto start = high_resolution_clock::now();

    for (size_t i = 0; i < iterations; ++i) {
        operation();
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    return static_cast<double>(duration.count()) / 1000.0 / iterations;
}

void benchmarkMapReduce(size_t data_size, const std::vector<size_t>& thread_counts) {
    std::vector<int> data(data_size);
    std::iota(data.begin(), data.end(), 0);

    auto map_fn = [](int x) { return x * x; };
    auto reduce_fn = [](long long a, long long b) { return a + b; };

    double baseline_time = 0.0;

    for (size_t num_threads : thread_counts) {
        double time = benchmarkOperation([&]() {
            Parallel::mapReduce(data, map_fn, reduce_fn, num_threads);
        });

        if (num_threads == 1) {
            baseline_time = time;
        }

        double speedup = (num_threads == 1 || baseline_time == 0.0) ? 1.0 : baseline_time / time;

        printResult({"Map-Reduce", data_size, num_threads, time, speedup});
    }
}

void benchmarkSort(size_t data_size, const std::vector<size_t>& thread_counts) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);

    double baseline_time = 0.0;

    for (size_t num_threads : thread_counts) {
        // Create fresh random data for each benchmark
        std::vector<int> data(data_size);
        for (auto& val : data) {
            val = dis(gen);
        }

        double time = benchmarkOperation([&]() {
            Parallel::parallelSort(data, num_threads);
        });

        if (num_threads == 1) {
            baseline_time = time;
        }

        double speedup = (num_threads == 1 || baseline_time == 0.0) ? 1.0 : baseline_time / time;

        printResult({"Parallel Sort", data_size, num_threads, time, speedup});
    }
}

void benchmarkAggregation(size_t data_size, const std::vector<size_t>& thread_counts) {
    std::vector<int> data(data_size);
    std::iota(data.begin(), data.end(), 1);

    double baseline_time = 0.0;

    for (size_t num_threads : thread_counts) {
        double time = benchmarkOperation([&]() {
            Parallel::parallelSum(data, num_threads);
            Parallel::parallelMin(data, num_threads);
            Parallel::parallelMax(data, num_threads);
            Parallel::parallelMean(data, num_threads);
        });

        if (num_threads == 1) {
            baseline_time = time;
        }

        double speedup = (num_threads == 1 || baseline_time == 0.0) ? 1.0 : baseline_time / time;

        printResult({"Aggregation", data_size, num_threads, time, speedup});
    }
}

void saveBenchmarkCSV(const std::string& filename,
                      const std::vector<BenchmarkResult>& results) {
    std::ofstream file(filename);
    file << "operation,data_size,num_threads,time_ms,speedup\n";

    for (const auto& result : results) {
        file << result.operation << ","
             << result.data_size << ","
             << result.num_threads << ","
             << result.execution_time_ms << ","
             << result.speedup << "\n";
    }

    file.close();
    std::cout << "\nResults saved to " << filename << std::endl;
}

int main(int argc, char* argv[]) {
    std::vector<size_t> thread_counts = {1, 2, 4, 8};
    size_t data_size = 10000000; // 10 million elements

    // Parse command line arguments
    if (argc > 1) {
        thread_counts.clear();
        for (int i = 1; i < argc; ++i) {
            thread_counts.push_back(std::atoi(argv[i]));
        }
    }

    std::cout << "Parallel Processing Benchmark Suite\n";
    std::cout << "====================================\n\n";

    printHeader();

    // Benchmark Map-Reduce
    benchmarkMapReduce(data_size, thread_counts);

    std::cout << std::endl;

    // Benchmark Parallel Sort
    benchmarkSort(data_size, thread_counts);

    std::cout << std::endl;

    // Benchmark Aggregation
    benchmarkAggregation(data_size, thread_counts);

    std::cout << "\n====================================\n";
    std::cout << "Hardware Concurrency: " << std::thread::hardware_concurrency() << " threads\n";

    return 0;
}
