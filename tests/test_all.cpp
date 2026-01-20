#include "parallel.h"
#include <iostream>
#include <cassert>
#include <numeric>
#include <random>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <chrono>
#include <atomic>

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

// ===========================================
// STRESS TESTS
// ===========================================

void test_stress_millions_of_elements() {
    std::cout << "Testing Stress: Millions of Elements... ";

    // Test with 5 million elements using int64_t to avoid overflow
    const size_t size = 5000000;
    std::vector<int64_t> data(size);
    std::iota(data.begin(), data.end(), int64_t(1));

    // Test parallel sum
    auto sum = Parallel::parallelSum(data, 8);
    // Sum of 1 to N = N * (N + 1) / 2
    int64_t expected = static_cast<int64_t>(size) * (size + 1) / 2;
    assert(sum == expected);

    // Test parallel sort with random int data (separate test)
    std::vector<int> sort_data(size);
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(1, 1000000);
    for (auto& val : sort_data) {
        val = dis(gen);
    }

    Parallel::parallelSort(sort_data, 8);
    assert(std::is_sorted(sort_data.begin(), sort_data.end()));

    std::cout << "PASSED" << std::endl;
}

void test_stress_int_max_values() {
    std::cout << "Testing Stress: INT_MAX Values... ";

    std::vector<int> data = {
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max() - 1,
        std::numeric_limits<int>::max() - 2,
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::min() + 1,
        0, -1, 1
    };

    // Test min/max with extreme values
    auto min_val = Parallel::parallelMin(data, 2);
    auto max_val = Parallel::parallelMax(data, 2);

    assert(min_val == std::numeric_limits<int>::min());
    assert(max_val == std::numeric_limits<int>::max());

    // Test sorting with extreme values
    Parallel::parallelSort(data, 4);
    assert(std::is_sorted(data.begin(), data.end()));
    assert(data.front() == std::numeric_limits<int>::min());
    assert(data.back() == std::numeric_limits<int>::max());

    std::cout << "PASSED" << std::endl;
}

void test_stress_negative_numbers() {
    std::cout << "Testing Stress: Negative Numbers... ";

    // Large dataset of negative numbers using int64_t to avoid overflow
    std::vector<int64_t> data(100000);
    std::mt19937 gen(42);
    std::uniform_int_distribution<int64_t> dis(-1000000, -1);

    for (auto& val : data) {
        val = dis(gen);
    }

    // Test sum (should be negative)
    auto sum = Parallel::parallelSum(data, 4);
    int64_t expected = std::accumulate(data.begin(), data.end(), int64_t(0));
    assert(sum == expected);

    // Test sorting negative numbers (use int for sort test)
    std::vector<int> sort_data(100000);
    std::uniform_int_distribution<int> dis_int(-1000000, -1);
    for (auto& val : sort_data) {
        val = dis_int(gen);
    }

    Parallel::parallelSort(sort_data, 4);
    assert(std::is_sorted(sort_data.begin(), sort_data.end()));
    assert(sort_data.front() < 0);
    assert(sort_data.back() < 0);

    // Test map-reduce with negative numbers
    auto map_fn = [](int x) { return static_cast<int64_t>(x) * x; }; // Squaring makes them positive
    auto reduce_fn = [](int64_t a, int64_t b) { return a + b; };

    std::vector<int> small_neg = {-1, -2, -3, -4, -5};
    auto result = Parallel::mapReduce(small_neg, map_fn, reduce_fn, 2);
    assert(result == 55); // 1 + 4 + 9 + 16 + 25

    std::cout << "PASSED" << std::endl;
}

void test_stress_floating_point_edge_cases() {
    std::cout << "Testing Stress: Floating-Point Edge Cases... ";

    // Test with special floating-point values
    std::vector<double> data = {
        0.0,
        -0.0,
        1.0,
        -1.0,
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::epsilon(),
        std::numeric_limits<double>::lowest()
    };

    // Test sum
    auto sum = Parallel::parallelSum(data, 2);
    double expected = std::accumulate(data.begin(), data.end(), 0.0);
    assert(std::abs(sum - expected) < 1e-10);

    // Test mean
    auto mean = Parallel::parallelMean(data, 2);
    assert(std::isfinite(mean));

    // Test min/max
    auto min_val = Parallel::parallelMin(data, 2);
    auto max_val = Parallel::parallelMax(data, 2);
    assert(min_val == std::numeric_limits<double>::lowest());
    assert(max_val == std::numeric_limits<double>::max());

    // Test with infinity (if included, operations should still work)
    std::vector<double> inf_data = {1.0, 2.0, 3.0, std::numeric_limits<double>::infinity()};
    auto inf_max = Parallel::parallelMax(inf_data, 2);
    assert(std::isinf(inf_max));

    // Test with very small differences
    std::vector<double> precise_data(10000);
    for (size_t i = 0; i < precise_data.size(); ++i) {
        precise_data[i] = 1.0 + (i * std::numeric_limits<double>::epsilon());
    }
    Parallel::parallelSort(precise_data, 4);
    assert(std::is_sorted(precise_data.begin(), precise_data.end()));

    std::cout << "PASSED" << std::endl;
}

// ===========================================
// THREAD CONTENTION TESTS
// ===========================================

void test_thread_contention_high_thread_count() {
    std::cout << "Testing Thread Contention: High Thread Count (32+)... ";

    // Test with 32+ threads on a reasonably sized dataset
    const size_t thread_counts[] = {32, 64, 128};
    const size_t data_size = 1000000;

    // Use int64_t to avoid overflow (sum of 1 to 1M is ~500 billion)
    std::vector<int64_t> data(data_size);
    std::iota(data.begin(), data.end(), int64_t(1));

    int64_t expected_sum = static_cast<int64_t>(data_size) * (data_size + 1) / 2;

    for (size_t num_threads : thread_counts) {
        // Test sum with high thread count
        auto sum = Parallel::parallelSum(data, num_threads);
        assert(sum == expected_sum);

        // Test map-reduce with high thread count
        auto map_fn = [](int64_t x) { return x; };
        auto reduce_fn = [](int64_t a, int64_t b) { return a + b; };
        auto mr_result = Parallel::mapReduce(data, map_fn, reduce_fn, num_threads);
        assert(mr_result == expected_sum);
    }

    // Test parallel sort with high thread count (separate int vector)
    std::vector<int> sort_data(data_size);
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(1, 1000000);
    for (auto& val : sort_data) {
        val = dis(gen);
    }

    Parallel::parallelSort(sort_data, 64);
    assert(std::is_sorted(sort_data.begin(), sort_data.end()));

    std::cout << "PASSED" << std::endl;
}

void test_thread_contention_rapid_submissions() {
    std::cout << "Testing Thread Contention: Rapid Submissions... ";

    Parallel::ThreadManager tm(8);

    const size_t num_tasks = 10000;
    std::vector<std::future<int>> futures;
    futures.reserve(num_tasks);

    std::atomic<int> counter{0};

    // Submit many tasks rapidly
    for (size_t i = 0; i < num_tasks; ++i) {
        futures.push_back(tm.submit([&counter, i]() {
            counter.fetch_add(1, std::memory_order_relaxed);
            return static_cast<int>(i);
        }));
    }

    // Verify all tasks completed correctly
    int sum = 0;
    for (size_t i = 0; i < num_tasks; ++i) {
        int result = futures[i].get();
        assert(result == static_cast<int>(i));
        sum += result;
    }

    // Verify counter
    assert(counter.load() == static_cast<int>(num_tasks));

    // Verify sum (0 + 1 + 2 + ... + (n-1) = n*(n-1)/2)
    int expected_sum = static_cast<int>(num_tasks * (num_tasks - 1) / 2);
    assert(sum == expected_sum);

    std::cout << "PASSED" << std::endl;
}

void test_thread_contention_repeated_operations() {
    std::cout << "Testing Thread Contention: Repeated Operations... ";

    const size_t iterations = 100;
    const size_t data_size = 10000;

    for (size_t iter = 0; iter < iterations; ++iter) {
        std::vector<int> data(data_size);
        std::iota(data.begin(), data.end(), 1);

        // Perform multiple parallel operations in quick succession
        auto sum = Parallel::parallelSum(data, 4);
        auto mean = Parallel::parallelMean(data, 4);
        auto min_val = Parallel::parallelMin(data, 4);
        auto max_val = Parallel::parallelMax(data, 4);

        int64_t expected_sum = static_cast<int64_t>(data_size) * (data_size + 1) / 2;
        assert(sum == expected_sum);
        assert(std::abs(mean - (data_size + 1) / 2.0) < 0.01);
        assert(min_val == 1);
        assert(max_val == static_cast<int>(data_size));
    }

    std::cout << "PASSED" << std::endl;
}

void test_thread_contention_concurrent_thread_managers() {
    std::cout << "Testing Thread Contention: Concurrent ThreadManagers... ";

    // Create multiple ThreadManagers simultaneously
    const size_t num_managers = 4;
    const size_t tasks_per_manager = 1000;

    std::vector<std::unique_ptr<Parallel::ThreadManager>> managers;
    std::vector<std::vector<std::future<int>>> all_futures(num_managers);

    for (size_t m = 0; m < num_managers; ++m) {
        managers.push_back(std::make_unique<Parallel::ThreadManager>(4));
        all_futures[m].reserve(tasks_per_manager);
    }

    // Submit tasks to all managers
    for (size_t m = 0; m < num_managers; ++m) {
        for (size_t t = 0; t < tasks_per_manager; ++t) {
            all_futures[m].push_back(managers[m]->submit([m, t]() {
                return static_cast<int>(m * tasks_per_manager + t);
            }));
        }
    }

    // Verify all results
    for (size_t m = 0; m < num_managers; ++m) {
        for (size_t t = 0; t < tasks_per_manager; ++t) {
            int expected = static_cast<int>(m * tasks_per_manager + t);
            int result = all_futures[m][t].get();
            assert(result == expected);
        }
    }

    std::cout << "PASSED" << std::endl;
}

// ===========================================
// EXCEPTION PROPAGATION TESTS
// ===========================================

void test_exception_propagation_map_function() {
    std::cout << "Testing Exception Propagation: Map Function... ";

    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Create a map function that throws on a specific value
    auto throwing_map = [](int x) -> int {
        if (x == 7) {
            throw std::runtime_error("Map function error on value 7");
        }
        return x * x;
    };
    auto reduce_fn = [](int a, int b) { return a + b; };

    bool exception_caught = false;
    try {
        // Use larger dataset to ensure parallel execution
        std::vector<int> large_data(10000);
        std::iota(large_data.begin(), large_data.end(), 1);
        Parallel::mapReduce(large_data, throwing_map, reduce_fn, 4);
    } catch (const std::runtime_error& e) {
        exception_caught = true;
        std::string msg(e.what());
        assert(msg.find("Map function error") != std::string::npos ||
               msg.find("error") != std::string::npos);
    }

    assert(exception_caught);
    std::cout << "PASSED" << std::endl;
}

void test_exception_propagation_reduce_function() {
    std::cout << "Testing Exception Propagation: Reduce Function... ";

    std::vector<int> data(10000);
    std::iota(data.begin(), data.end(), 1);

    auto map_fn = [](int x) { return x; };

    // Create a reduce function that throws when sum exceeds threshold
    auto throwing_reduce = [](int64_t a, int64_t b) -> int64_t {
        int64_t result = a + b;
        if (result > 1000000) {
            throw std::runtime_error("Reduce function overflow");
        }
        return result;
    };

    bool exception_caught = false;
    try {
        Parallel::mapReduce(data, map_fn, throwing_reduce, 4);
    } catch (const std::runtime_error& e) {
        exception_caught = true;
    }

    assert(exception_caught);
    std::cout << "PASSED" << std::endl;
}

void test_exception_propagation_thread_manager() {
    std::cout << "Testing Exception Propagation: ThreadManager... ";

    Parallel::ThreadManager tm(4);

    // Submit a task that throws
    auto future = tm.submit([]() -> int {
        throw std::runtime_error("Task exception");
        return 42;
    });

    bool exception_caught = false;
    try {
        future.get();
    } catch (const std::runtime_error& e) {
        exception_caught = true;
        std::string msg(e.what());
        assert(msg.find("Task exception") != std::string::npos);
    }

    assert(exception_caught);

    // Verify ThreadManager still works after exception
    auto future2 = tm.submit([]() { return 100; });
    assert(future2.get() == 100);

    std::cout << "PASSED" << std::endl;
}

void test_exception_propagation_multiple_exceptions() {
    std::cout << "Testing Exception Propagation: Multiple Exceptions... ";

    Parallel::ThreadManager tm(4);

    const size_t num_tasks = 100;
    std::vector<std::future<int>> futures;
    futures.reserve(num_tasks);

    // Submit mix of throwing and non-throwing tasks
    for (size_t i = 0; i < num_tasks; ++i) {
        if (i % 10 == 5) {
            futures.push_back(tm.submit([i]() -> int {
                throw std::runtime_error("Exception from task " + std::to_string(i));
            }));
        } else {
            futures.push_back(tm.submit([i]() -> int {
                return static_cast<int>(i);
            }));
        }
    }

    size_t exceptions_caught = 0;
    size_t successful_results = 0;

    for (size_t i = 0; i < num_tasks; ++i) {
        try {
            int result = futures[i].get();
            assert(result == static_cast<int>(i));
            successful_results++;
        } catch (const std::runtime_error&) {
            exceptions_caught++;
        }
    }

    // 10% of tasks should throw (every 10th starting at 5)
    assert(exceptions_caught == 10);
    assert(successful_results == 90);

    std::cout << "PASSED" << std::endl;
}

void test_exception_propagation_async_map_reduce() {
    std::cout << "Testing Exception Propagation: Async MapReduce... ";

    std::vector<int> data(10000);
    std::iota(data.begin(), data.end(), 1);

    auto throwing_map = [](int x) -> int {
        if (x == 5000) {
            throw std::runtime_error("Async map error");
        }
        return x;
    };
    auto reduce_fn = [](int a, int b) { return a + b; };

    auto future = Parallel::mapReduceAsync(data, throwing_map, reduce_fn, 4);

    bool exception_caught = false;
    try {
        future.get();
    } catch (const std::runtime_error&) {
        exception_caught = true;
    }

    assert(exception_caught);
    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "Running Parallel Processing Library Tests\n";
    std::cout << "==========================================\n";
    std::cout << "Hardware Concurrency: " << std::thread::hardware_concurrency() << " threads\n\n";

    try {
        // Original tests
        std::cout << "--- Basic Functionality Tests ---\n";
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

        // Stress tests
        std::cout << "\n--- Stress Tests ---\n";
        test_stress_millions_of_elements();
        test_stress_int_max_values();
        test_stress_negative_numbers();
        test_stress_floating_point_edge_cases();

        // Thread contention tests
        std::cout << "\n--- Thread Contention Tests ---\n";
        test_thread_contention_high_thread_count();
        test_thread_contention_rapid_submissions();
        test_thread_contention_repeated_operations();
        test_thread_contention_concurrent_thread_managers();

        // Exception propagation tests
        std::cout << "\n--- Exception Propagation Tests ---\n";
        test_exception_propagation_map_function();
        test_exception_propagation_reduce_function();
        test_exception_propagation_thread_manager();
        test_exception_propagation_multiple_exceptions();
        test_exception_propagation_async_map_reduce();

        std::cout << "\n==========================================\n";
        std::cout << "All tests PASSED!\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTest FAILED with exception: " << e.what() << std::endl;
        return 1;
    }
}
