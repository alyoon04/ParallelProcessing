#include "thread_manager.h"
#include <stdexcept>

namespace Parallel {

ThreadManager::ThreadManager(size_t num_threads)
    : stop_(false), chunk_size_(1000), num_threads_(num_threads) {

    if (num_threads == 0) {
        num_threads_ = std::thread::hardware_concurrency();
        if (num_threads_ == 0) num_threads_ = 2;
    }

    workers_.reserve(num_threads_);
    for (size_t i = 0; i < num_threads_; ++i) {
        workers_.emplace_back(&ThreadManager::workerThread, this);
    }
}

ThreadManager::~ThreadManager() {
    shutdown();
}

void ThreadManager::workerThread() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            condition_.wait(lock, [this] {
                return stop_ || !tasks_.empty();
            });

            if (stop_ && tasks_.empty()) {
                return;
            }

            if (!tasks_.empty()) {
                task = std::move(tasks_.front());
                tasks_.pop();
            }
        }

        if (task) {
            task();
        }
    }
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
        std::unique_lock<std::mutex> lock(queue_mutex_);
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
