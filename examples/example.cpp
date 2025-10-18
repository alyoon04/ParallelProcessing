#include "parallel.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <random>

void example_map_reduce() {
    std::cout << "=== Map-Reduce Example ===\n";

    std::vector<int> data(1000000);
    std::iota(data.begin(), data.end(), 0);

    auto map_fn = [](int x) { return x * x; };
    auto reduce_fn = [](long long a, long long b) { return a + b; };

    auto result = Parallel::mapReduce(data, map_fn, reduce_fn, 8);

    std::cout << "Sum of squares (0 to 999999): " << result << std::endl;
    std::cout << std::endl;
}

void example_parallel_sort() {
    std::cout << "=== Parallel Sort Example ===\n";

    std::vector<int> data(100);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000);

    for (auto& val : data) {
        val = dis(gen);
    }

    std::cout << "Before sort (first 10): ";
    for (size_t i = 0; i < 10; ++i) {
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;

    Parallel::parallelSort(data, 4);

    std::cout << "After sort (first 10):  ";
    for (size_t i = 0; i < 10; ++i) {
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;

    // Verify sort
    bool is_sorted = std::is_sorted(data.begin(), data.end());
    std::cout << "Is sorted: " << (is_sorted ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
}

void example_aggregation() {
    std::cout << "=== Parallel Aggregation Example ===\n";

    std::vector<int> data(1000000);
    std::iota(data.begin(), data.end(), 1);

    auto sum = Parallel::parallelSum(data, 8);
    auto mean = Parallel::parallelMean(data, 8);
    auto min_val = Parallel::parallelMin(data, 8);
    auto max_val = Parallel::parallelMax(data, 8);

    std::cout << "Data size: " << data.size() << std::endl;
    std::cout << "Sum:  " << sum << std::endl;
    std::cout << "Mean: " << mean << std::endl;
    std::cout << "Min:  " << min_val << std::endl;
    std::cout << "Max:  " << max_val << std::endl;
    std::cout << std::endl;
}

void example_custom_reduce() {
    std::cout << "=== Custom Reduce Example ===\n";

    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Custom reducer: multiply all elements
    auto multiply_fn = [](int a, int b) { return a * b; };

    auto product = Parallel::parallelReduce(data, multiply_fn, 1, 4);

    std::cout << "Product of [1..10]: " << product << std::endl;
    std::cout << std::endl;
}

void example_async_map_reduce() {
    std::cout << "=== Async Map-Reduce Example ===\n";

    std::vector<int> data(500000);
    std::iota(data.begin(), data.end(), 0);

    auto map_fn = [](int x) { return x * 2; };
    auto reduce_fn = [](long long a, long long b) { return a + b; };

    // Launch async computation
    auto future = Parallel::mapReduceAsync(data, map_fn, reduce_fn, 4);

    std::cout << "Computation launched asynchronously..." << std::endl;
    std::cout << "Doing other work while waiting..." << std::endl;

    // Get result when ready
    auto result = future.get();

    std::cout << "Sum of (2 * x) for x in [0..499999]: " << result << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "Parallel Processing Library - Examples\n";
    std::cout << "========================================\n";
    std::cout << "Hardware Concurrency: " << std::thread::hardware_concurrency() << " threads\n\n";

    example_map_reduce();
    example_parallel_sort();
    example_aggregation();
    example_custom_reduce();
    example_async_map_reduce();

    std::cout << "All examples completed successfully!\n";

    return 0;
}
