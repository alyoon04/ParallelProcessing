#ifndef PARALLEL_SORT_H
#define PARALLEL_SORT_H

#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include <functional>

namespace Parallel {

namespace detail {
    // Helper function to determine optimal recursion depth
    inline size_t getMaxDepth(size_t num_threads) {
        size_t depth = 0;
        while ((1ULL << depth) < num_threads) {
            ++depth;
        }
        return depth;
    }

    template<typename Iterator, typename Compare>
    void parallelSortImpl(Iterator begin, Iterator end, Compare comp,
                         size_t depth, size_t max_depth) {

        size_t size = std::distance(begin, end);

        // Base case: sort serially if small or max depth reached
        if (size < 10000 || depth >= max_depth) {
            std::sort(begin, end, comp);
            return;
        }

        // Find middle point
        Iterator mid = begin + size / 2;

        // Recursively sort both halves in parallel
        auto left_future = std::async(std::launch::async,
            parallelSortImpl<Iterator, Compare>,
            begin, mid, comp, depth + 1, max_depth
        );

        parallelSortImpl(mid, end, comp, depth + 1, max_depth);

        // Wait for left half to complete
        left_future.get();

        // Merge the two sorted halves
        std::inplace_merge(begin, mid, end, comp);
    }
}

// Forward declaration
template<typename T, typename Compare>
void parallelSort(std::vector<T>& data, Compare comp, size_t num_threads);

template<typename T>
void parallelSort(std::vector<T>& data, size_t num_threads) {
    parallelSort(data, std::less<T>(), num_threads);
}

template<typename T, typename Compare>
void parallelSort(std::vector<T>& data, Compare comp, size_t num_threads) {
    if (data.empty()) return;

    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 2;
    }

    // For small datasets, use serial sort
    if (data.size() < 10000 || num_threads == 1) {
        std::sort(data.begin(), data.end(), comp);
        return;
    }

    size_t max_depth = detail::getMaxDepth(num_threads);
    detail::parallelSortImpl(data.begin(), data.end(), comp, 0, max_depth);
}

} // namespace Parallel

#endif // PARALLEL_SORT_H
