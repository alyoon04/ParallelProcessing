#include "parallel.h"
#include <iostream>
#include <cassert>
#include <numeric>
#include <random>
#include <cmath>

void test_map_reduce() {
    std::cout << "Testing Map-Reduce... ";

    std::vector<int> data = {1, 2, 3, 4, 5};

    auto map_fn = [](int x) { return x * x; };
    auto reduce_fn = [](int a, int b) { return a + b; };

    auto result = Parallel::mapReduce(data, map_fn, reduce_fn, 2);

    // 1^2 + 2^2 + 3^2 + 4^2 + 5^2 = 1 + 4 + 9 + 16 + 25 = 55
    assert(result == 55);

    std::cout << "PASSED" << std::endl;
}

void test_parallel_sort() {
    std::cout << "Testing Parallel Sort... ";

    std::vector<int> data = {5, 2, 8, 1, 9, 3, 7, 4, 6};

    Parallel::parallelSort(data, 2);

    assert(std::is_sorted(data.begin(), data.end()));
    assert(data[0] == 1);
    assert(data[8] == 9);

    std::cout << "PASSED" << std::endl;
}

void test_parallel_sort_large() {
    std::cout << "Testing Parallel Sort (Large)... ";

    std::vector<int> data(100000);
    std::random_device rd;
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<> dis(1, 1000000);

    for (auto& val : data) {
        val = dis(gen);
    }

    Parallel::parallelSort(data, 4);

    assert(std::is_sorted(data.begin(), data.end()));

    std::cout << "PASSED" << std::endl;
}

void test_parallel_sum() {
    std::cout << "Testing Parallel Sum... ";

    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 1);

    auto result = Parallel::parallelSum(data, 4);

    // Sum of 1 to 1000 = 1000 * 1001 / 2 = 500500
    assert(result == 500500);

    std::cout << "PASSED" << std::endl;
}

void test_parallel_mean() {
    std::cout << "Testing Parallel Mean... ";

    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto result = Parallel::parallelMean(data, 2);

    assert(std::abs(result - 5.5) < 0.001);

    std::cout << "PASSED" << std::endl;
}

void test_parallel_min_max() {
    std::cout << "Testing Parallel Min/Max... ";

    std::vector<int> data = {5, 2, 8, 1, 9, 3, 7, 4, 6};

    auto min_val = Parallel::parallelMin(data, 2);
    auto max_val = Parallel::parallelMax(data, 2);

    assert(min_val == 1);
    assert(max_val == 9);

    std::cout << "PASSED" << std::endl;
}

void test_parallel_reduce() {
    std::cout << "Testing Parallel Reduce... ";

    std::vector<int> data = {1, 2, 3, 4, 5};

    auto multiply_fn = [](int a, int b) { return a * b; };

    auto result = Parallel::parallelReduce(data, multiply_fn, 1, 2);

    // 1 * 2 * 3 * 4 * 5 = 120
    assert(result == 120);

    std::cout << "PASSED" << std::endl;
}

void test_async_map_reduce() {
    std::cout << "Testing Async Map-Reduce... ";

    std::vector<int> data = {1, 2, 3, 4, 5};

    auto map_fn = [](int x) { return x * 2; };
    auto reduce_fn = [](int a, int b) { return a + b; };

    auto future = Parallel::mapReduceAsync(data, map_fn, reduce_fn, 2);
    auto result = future.get();

    // (1*2) + (2*2) + (3*2) + (4*2) + (5*2) = 2 + 4 + 6 + 8 + 10 = 30
    assert(result == 30);

    std::cout << "PASSED" << std::endl;
}

void test_empty_data() {
    std::cout << "Testing Empty Data Handling... ";

    std::vector<int> empty_data;

    // Test sum with empty data
    auto sum = Parallel::parallelSum(empty_data, 2);
    assert(sum == 0);

    // Test mean with empty data
    auto mean = Parallel::parallelMean(empty_data, 2);
    assert(mean == 0.0);

    std::cout << "PASSED" << std::endl;
}

void test_single_thread() {
    std::cout << "Testing Single Thread Mode... ";

    std::vector<int> data = {5, 2, 8, 1, 9};

    // Test with 1 thread (should use serial path)
    Parallel::parallelSort(data, 1);
    assert(std::is_sorted(data.begin(), data.end()));

    std::vector<int> data2 = {1, 2, 3, 4, 5};
    auto map_fn = [](int x) { return x * x; };
    auto reduce_fn = [](int a, int b) { return a + b; };
    auto result = Parallel::mapReduce(data2, map_fn, reduce_fn, 1);
    assert(result == 55);

    std::cout << "PASSED" << std::endl;
}

void test_thread_manager() {
    std::cout << "Testing ThreadManager... ";

    Parallel::ThreadManager tm(4);

    assert(tm.getNumThreads() == 4);

    tm.setChunkSize(500);
    assert(tm.getChunkSize() == 500);

    // Submit a simple task
    auto future = tm.submit([]() { return 42; });
    assert(future.get() == 42);

    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "Running Parallel Processing Library Tests\n";
    std::cout << "==========================================\n";
    std::cout << "Hardware Concurrency: " << std::thread::hardware_concurrency() << " threads\n\n";

    try {
        test_map_reduce();
        test_parallel_sort();
        test_parallel_sort_large();
        test_parallel_sum();
        test_parallel_mean();
        test_parallel_min_max();
        test_parallel_reduce();
        test_async_map_reduce();
        test_empty_data();
        test_single_thread();
        test_thread_manager();

        std::cout << "\n==========================================\n";
        std::cout << "All tests PASSED!\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTest FAILED with exception: " << e.what() << std::endl;
        return 1;
    }
}
