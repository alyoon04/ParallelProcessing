#ifndef PARALLEL_AGGREGATION_H
#define PARALLEL_AGGREGATION_H

#include <vector>
#include <thread>
#include <future>
#include <algorithm>
#include <numeric>
#include <limits>

namespace Parallel {

namespace detail {
    template<typename T, typename ReduceFunc>
    T reduceChunk(typename std::vector<T>::const_iterator begin,
                  typename std::vector<T>::const_iterator end,
                  ReduceFunc reduce_fn,
                  T init) {
        T result = init;
        for (auto it = begin; it != end; ++it) {
            result = reduce_fn(result, *it);
        }
        return result;
    }

    template<typename T>
    T sumChunk(typename std::vector<T>::const_iterator begin,
               typename std::vector<T>::const_iterator end) {
        T sum = T();
        for (auto it = begin; it != end; ++it) {
            sum += *it;
        }
        return sum;
    }

    template<typename T>
    T minChunk(typename std::vector<T>::const_iterator begin,
               typename std::vector<T>::const_iterator end) {
        if (begin == end) {
            return std::numeric_limits<T>::max();
        }
        return *std::min_element(begin, end);
    }

    template<typename T>
    T maxChunk(typename std::vector<T>::const_iterator begin,
               typename std::vector<T>::const_iterator end) {
        if (begin == end) {
            return std::numeric_limits<T>::lowest();
        }
        return *std::max_element(begin, end);
    }
}

template<typename T, typename ReduceFunc>
T parallelReduce(const std::vector<T>& data, ReduceFunc reduce_fn, T init, size_t num_threads) {
    if (data.empty()) {
        return init;
    }

    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 2;
    }

    // For small datasets, use serial reduction
    if (data.size() < 1000 || num_threads == 1) {
        return detail::reduceChunk<T>(data.begin(), data.end(), reduce_fn, init);
    }

    // Calculate chunk size
    size_t chunk_size = (data.size() + num_threads - 1) / num_threads;

    // Launch worker threads
    std::vector<std::future<T>> futures;
    futures.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start_idx = i * chunk_size;
        size_t end_idx = std::min(start_idx + chunk_size, data.size());

        if (start_idx >= data.size()) break;

        futures.push_back(std::async(std::launch::async,
            detail::reduceChunk<T, ReduceFunc>,
            data.begin() + start_idx,
            data.begin() + end_idx,
            reduce_fn,
            init
        ));
    }

    // Collect and reduce results
    T result = init;
    for (auto& future : futures) {
        result = reduce_fn(result, future.get());
    }

    return result;
}

template<typename T>
T parallelSum(const std::vector<T>& data, size_t num_threads) {
    if (data.empty()) {
        return T();
    }

    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 2;
    }

    // For small datasets, use serial sum
    if (data.size() < 1000 || num_threads == 1) {
        return std::accumulate(data.begin(), data.end(), T());
    }

    // Calculate chunk size
    size_t chunk_size = (data.size() + num_threads - 1) / num_threads;

    // Launch worker threads
    std::vector<std::future<T>> futures;
    futures.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start_idx = i * chunk_size;
        size_t end_idx = std::min(start_idx + chunk_size, data.size());

        if (start_idx >= data.size()) break;

        futures.push_back(std::async(std::launch::async,
            detail::sumChunk<T>,
            data.begin() + start_idx,
            data.begin() + end_idx
        ));
    }

    // Collect and sum results
    T result = T();
    for (auto& future : futures) {
        result += future.get();
    }

    return result;
}

template<typename T>
double parallelMean(const std::vector<T>& data, size_t num_threads) {
    if (data.empty()) {
        return 0.0;
    }

    T sum = parallelSum(data, num_threads);
    return static_cast<double>(sum) / static_cast<double>(data.size());
}

template<typename T>
T parallelMin(const std::vector<T>& data, size_t num_threads) {
    if (data.empty()) {
        throw std::runtime_error("Cannot find minimum of empty data");
    }

    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 2;
    }

    // For small datasets, use serial min
    if (data.size() < 1000 || num_threads == 1) {
        return *std::min_element(data.begin(), data.end());
    }

    // Calculate chunk size
    size_t chunk_size = (data.size() + num_threads - 1) / num_threads;

    // Launch worker threads
    std::vector<std::future<T>> futures;
    futures.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start_idx = i * chunk_size;
        size_t end_idx = std::min(start_idx + chunk_size, data.size());

        if (start_idx >= data.size()) break;

        futures.push_back(std::async(std::launch::async,
            detail::minChunk<T>,
            data.begin() + start_idx,
            data.begin() + end_idx
        ));
    }

    // Find minimum across all results
    T result = std::numeric_limits<T>::max();
    for (auto& future : futures) {
        result = std::min(result, future.get());
    }

    return result;
}

template<typename T>
T parallelMax(const std::vector<T>& data, size_t num_threads) {
    if (data.empty()) {
        throw std::runtime_error("Cannot find maximum of empty data");
    }

    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 2;
    }

    // For small datasets, use serial max
    if (data.size() < 1000 || num_threads == 1) {
        return *std::max_element(data.begin(), data.end());
    }

    // Calculate chunk size
    size_t chunk_size = (data.size() + num_threads - 1) / num_threads;

    // Launch worker threads
    std::vector<std::future<T>> futures;
    futures.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start_idx = i * chunk_size;
        size_t end_idx = std::min(start_idx + chunk_size, data.size());

        if (start_idx >= data.size()) break;

        futures.push_back(std::async(std::launch::async,
            detail::maxChunk<T>,
            data.begin() + start_idx,
            data.begin() + end_idx
        ));
    }

    // Find maximum across all results
    T result = std::numeric_limits<T>::lowest();
    for (auto& future : futures) {
        result = std::max(result, future.get());
    }

    return result;
}

} // namespace Parallel

#endif // PARALLEL_AGGREGATION_H
