#pragma once

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class tsQueue {
   public:
    tsQueue(const T& limiter);
    // tsQueue(T&& limiter);
    void push(T value);
    bool wait_and_pop(T& data);
    void wake_and_done();
    [[nodiscard]] bool empty() const;

   private:
    const T limiter_;
    mutable std::mutex mut_;
    std::condition_variable cond_var;
    std::queue<T> q_;
};

#include "ts_queue.inl"