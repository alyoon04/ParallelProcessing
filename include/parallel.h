#ifndef PARALLEL_H
#define PARALLEL_H

#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <algorithm>
#include <numeric>
#include <memory>

namespace Parallel {
    // Forward declaration
    class ThreadManager;
} // namespace Parallel

// Include template implementations
#include "thread_manager.h"
#include "map_reduce.h"
#include "parallel_sort.h"
#include "parallel_aggregation.h"

#endif // PARALLEL_H
