#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <random>
#include "work_stealing_deque.h"

namespace Parallel {

class ThreadManager {
public:
    explicit ThreadManager(size_t num_threads);
    ~ThreadManager();

    // Delete copy constructor and assignment
    ThreadManager(const ThreadManager&) = delete;
    ThreadManager& operator=(const ThreadManager&) = delete;

    // Submit a task to the thread pool
    template<typename Func, typename... Args>
    auto submit(Func&& func, Args&&... args)
        -> std::future<typename std::invoke_result<Func, Args...>::type>;

    // Set chunk size for workload distribution
    void setChunkSize(size_t chunk_size);
    size_t getChunkSize() const;

    // Get number of threads
    size_t getNumThreads() const;

    // Shutdown the thread pool
    void shutdown();

private:
    std::vector<std::thread> workers_;
    std::vector<std::unique_ptr<WorkStealingDeque>> deques_;  // Per-thread deques

    std::mutex wake_mutex_;
    std::condition_variable condition_;

    std::atomic<bool> stop_;
    std::atomic<size_t> chunk_size_;
    std::atomic<size_t> next_submit_index_;  // Round-robin submission
    size_t num_threads_;

    void workerThread(size_t worker_id);

    // Try to steal a task from another worker's deque
    std::optional<std::function<void()>> trySteal(size_t worker_id);
};

// Template implementation
template<typename Func, typename... Args>
auto ThreadManager::submit(Func&& func, Args&&... args)
    -> std::future<typename std::invoke_result<Func, Args...>::type> {

    using return_type = typename std::invoke_result<Func, Args...>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
    );

    std::future<return_type> result = task->get_future();

    if (stop_) {
        throw std::runtime_error("Cannot submit task to stopped ThreadManager");
    }

    // Round-robin distribution to worker deques
    size_t target = next_submit_index_.fetch_add(1, std::memory_order_relaxed) % num_threads_;
    deques_[target]->push([task]() { (*task)(); });

    // Wake one worker (they will steal if needed)
    condition_.notify_one();

    return result;
}

} // namespace Parallel

#endif // THREAD_MANAGER_H
