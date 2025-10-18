#ifndef MAP_REDUCE_H
#define MAP_REDUCE_H

#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <future>
#include <algorithm>

namespace Parallel {

namespace detail {
    template<typename T, typename MapFunc, typename ReduceFunc>
    auto mapReduceChunk(
        typename std::vector<T>::const_iterator begin,
        typename std::vector<T>::const_iterator end,
        MapFunc map_fn,
        ReduceFunc reduce_fn) -> decltype(map_fn(*begin)) {

        if (begin == end) {
            throw std::runtime_error("Empty chunk in mapReduce");
        }

        auto result = map_fn(*begin);
        ++begin;

        for (auto it = begin; it != end; ++it) {
            result = reduce_fn(result, map_fn(*it));
        }

        return result;
    }
}

template<typename T, typename MapFunc, typename ReduceFunc>
auto mapReduce(const std::vector<T>& data, MapFunc map_fn, ReduceFunc reduce_fn, size_t num_threads)
    -> decltype(map_fn(data[0])) {

    if (data.empty()) {
        throw std::runtime_error("Cannot perform mapReduce on empty data");
    }

    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 2;
    }

    // If dataset is small or only one thread, process serially
    if (data.size() < 1000 || num_threads == 1) {
        return detail::mapReduceChunk<T>(data.begin(), data.end(), map_fn, reduce_fn);
    }

    // Calculate chunk size
    size_t chunk_size = (data.size() + num_threads - 1) / num_threads;

    // Launch worker threads
    std::vector<std::future<decltype(map_fn(data[0]))>> futures;
    futures.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start_idx = i * chunk_size;
        size_t end_idx = std::min(start_idx + chunk_size, data.size());

        if (start_idx >= data.size()) break;

        futures.push_back(std::async(std::launch::async,
            detail::mapReduceChunk<T, MapFunc, ReduceFunc>,
            data.begin() + start_idx,
            data.begin() + end_idx,
            map_fn,
            reduce_fn
        ));
    }

    // Collect and reduce results from all threads
    auto final_result = futures[0].get();

    for (size_t i = 1; i < futures.size(); ++i) {
        final_result = reduce_fn(final_result, futures[i].get());
    }

    return final_result;
}

template<typename T, typename MapFunc, typename ReduceFunc>
auto mapReduceAsync(const std::vector<T>& data, MapFunc map_fn, ReduceFunc reduce_fn, size_t num_threads)
    -> std::future<decltype(mapReduce(data, map_fn, reduce_fn, num_threads))> {

    return std::async(std::launch::async, mapReduce<T, MapFunc, ReduceFunc>,
                      std::cref(data), map_fn, reduce_fn, num_threads);
}

} // namespace Parallel

#endif // MAP_REDUCE_H
