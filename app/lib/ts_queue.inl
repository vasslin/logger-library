#include "ts_queue.h"

template <typename T>
tsQueue<T>::tsQueue(const T& limiter) : limiter_(limiter) {}

template <typename T>
void tsQueue<T>::push(T value) {
    std::lock_guard<std::mutex> lk{mut_};
    q_.push(std::move(value));
    cond_var.notify_one();
}

template <typename T>
bool tsQueue<T>::wait_and_pop(T& data) {
    std::unique_lock<std::mutex> ul{mut_};
    cond_var.wait(ul, [this]() { return !q_.empty(); });

    if (q_.front() == limiter_) {  // end of setting elements
        return false;             // queue was empty
    }
    data = std::move(q_.front());
    q_.pop();
    return true;  // queue was not empty
}

template <typename T>
void tsQueue<T>::wake_and_done() {
    std::lock_guard<std::mutex> lk{mut_};
    q_.push(limiter_);
    cond_var.notify_all();
}

template <typename T>
bool tsQueue<T>::empty() const {
    std::lock_guard<std::mutex> lk{mut_};
    return q_.empty();
}
