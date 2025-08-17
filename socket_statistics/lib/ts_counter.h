
#include <thread>
#include <mutex>

template <typename T>
class tsCounter {
   public:
    tsCounter() : value_(T()) {}
    tsCounter(T&& value) : value_(std::move(value)) {}
    tsCounter(const T& value) : value_(value) {}

    tsCounter& operator++() {
        std::lock_guard<std::mutex> lk{mut_};
        ++value_;
        return *this;
    };
    tsCounter& operator--() {
        std::lock_guard<std::mutex> lk{mut_};
        --value_;
        return *this;
    };
    void set(T&& val) {
        std::lock_guard<std::mutex> lk{mut_};
        value_ = std::move(val);
    }
    void set(const T& val) {
        std::lock_guard<std::mutex> lk{mut_};
        value_ = val;
    }
    T get() {
        std::lock_guard<std::mutex> lk{mut_};
        return value_;
    }

   private:
    T value_;
    std::mutex mut_;
};