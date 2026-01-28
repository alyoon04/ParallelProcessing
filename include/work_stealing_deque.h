#ifndef WORK_STEALING_DEQUE_H
#define WORK_STEALING_DEQUE_H

#include <deque>
#include <mutex>
#include <optional>
#include <functional>

namespace Parallel {

// Work-stealing deque implementation
// Owner thread: push/pop from bottom (LIFO) - fast path with minimal contention
// Thieves: steal from top (FIFO) - may contend with other thieves
class WorkStealingDeque {
public:
    using Task = std::function<void()>;

    WorkStealingDeque() = default;

    // Non-copyable, non-movable (contains mutex)
    WorkStealingDeque(const WorkStealingDeque&) = delete;
    WorkStealingDeque& operator=(const WorkStealingDeque&) = delete;
    WorkStealingDeque(WorkStealingDeque&&) = delete;
    WorkStealingDeque& operator=(WorkStealingDeque&&) = delete;

    // Push task to bottom (called by owner thread)
    void push(Task task) {
        std::lock_guard<std::mutex> lock(mutex_);
        deque_.push_back(std::move(task));
    }

    // Pop task from bottom (called by owner thread) - LIFO
    std::optional<Task> pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (deque_.empty()) {
            return std::nullopt;
        }
        Task task = std::move(deque_.back());
        deque_.pop_back();
        return task;
    }

    // Steal task from top (called by thief threads) - FIFO
    std::optional<Task> steal() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (deque_.empty()) {
            return std::nullopt;
        }
        Task task = std::move(deque_.front());
        deque_.pop_front();
        return task;
    }

    // Check if empty (approximate - may race)
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return deque_.empty();
    }

    // Get size (approximate - may race)
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return deque_.size();
    }

private:
    mutable std::mutex mutex_;
    std::deque<Task> deque_;
};

} // namespace Parallel

#endif // WORK_STEALING_DEQUE_H
