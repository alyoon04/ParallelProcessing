#include "thread_manager.h"
#include <stdexcept>

namespace Parallel {

ThreadManager::ThreadManager(size_t num_threads)
    : stop_(false), chunk_size_(1000), next_submit_index_(0), num_threads_(num_threads) {

    if (num_threads == 0) {
        num_threads_ = std::thread::hardware_concurrency();
        if (num_threads_ == 0) num_threads_ = 2;
    }

    // Create per-thread work-stealing deques
    deques_.reserve(num_threads_);
    for (size_t i = 0; i < num_threads_; ++i) {
        deques_.push_back(std::make_unique<WorkStealingDeque>());
    }

    // Create worker threads
    workers_.reserve(num_threads_);
    for (size_t i = 0; i < num_threads_; ++i) {
        workers_.emplace_back(&ThreadManager::workerThread, this, i);
    }
}

ThreadManager::~ThreadManager() {
    shutdown();
}

void ThreadManager::workerThread(size_t worker_id) {
    // Simple RNG for randomized stealing
    std::mt19937 rng(static_cast<unsigned>(worker_id * 12345 + 67890));

    while (true) {
        std::function<void()> task;

        // First, try to pop from own deque (fast path, no global contention)
        auto local_task = deques_[worker_id]->pop();
        if (local_task) {
            task = std::move(*local_task);
        } else {
            // Own deque empty, try to steal from others
            auto stolen = trySteal(worker_id);
            if (stolen) {
                task = std::move(*stolen);
            } else {
                // No work available anywhere, wait for notification
                std::unique_lock<std::mutex> lock(wake_mutex_);

                // Check for termination
                if (stop_) {
                    // Before exiting, drain any remaining tasks in own deque
                    while (auto remaining = deques_[worker_id]->pop()) {
                        (*remaining)();
                    }
                    return;
                }

                // Wait with timeout to allow periodic steal attempts
                condition_.wait_for(lock, std::chrono::microseconds(100), [this, worker_id] {
                    return stop_ || !deques_[worker_id]->empty();
                });

                // After waking, continue the loop to try local pop or steal
                continue;
            }
        }

        // Execute the task
        if (task) {
            task();
        }
    }
}

std::optional<std::function<void()>> ThreadManager::trySteal(size_t worker_id) {
    // Try to steal from other workers' deques
    // Start from a random position to avoid everyone stealing from the same victim
    size_t start = worker_id;

    for (size_t i = 1; i < num_threads_; ++i) {
        size_t victim = (start + i) % num_threads_;
        auto stolen = deques_[victim]->steal();
        if (stolen) {
            return stolen;
        }
    }

    return std::nullopt;
}

void ThreadManager::setChunkSize(size_t chunk_size) {
    chunk_size_ = chunk_size;
}

size_t ThreadManager::getChunkSize() const {
    return chunk_size_;
}

size_t ThreadManager::getNumThreads() const {
    return num_threads_;
}

void ThreadManager::shutdown() {
    {
        std::unique_lock<std::mutex> lock(wake_mutex_);
        if (stop_) {
            return;  // Already shut down
        }
        stop_ = true;
    }

    condition_.notify_all();

    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

} // namespace Parallel
