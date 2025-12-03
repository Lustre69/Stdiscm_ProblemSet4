// Simple bounded thread-safe queue for image items
#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

template<typename T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue(size_t max_size = 0) : max_size_(max_size), finished_(false) {}

    // push item; if bounded and full, wait
    void push(T item) {
        std::unique_lock<std::mutex> lk(m_);
        if (max_size_ > 0) {
            cv_full_.wait(lk, [&]() { return queue_.size() < max_size_ || finished_; });
        }
        queue_.push(std::move(item));
        lk.unlock();
        cv_.notify_one();
    }

    // pop item; returns false if finished and queue empty
    bool pop(T &out) {
        std::unique_lock<std::mutex> lk(m_);
        cv_.wait(lk, [&]() { return !queue_.empty() || finished_; });
        if (queue_.empty()) return false;
        out = std::move(queue_.front());
        queue_.pop();
        lk.unlock();
        cv_full_.notify_one();
        return true;
    }

    void set_finished() {
        std::lock_guard<std::mutex> lk(m_);
        finished_ = true;
        cv_.notify_all();
        cv_full_.notify_all();
    }

    bool empty() {
        std::lock_guard<std::mutex> lk(m_);
        return queue_.empty();
    }

private:
    std::queue<T> queue_;
    size_t max_size_;
    bool finished_;
    std::mutex m_;
    std::condition_variable cv_;
    std::condition_variable cv_full_;
};
