#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

template <typename T>
class threadsafe_queue {
public:
    threadsafe_queue() = default;

    threadsafe_queue(const threadsafe_queue& other)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _data = other._data;
    }

    void push(T value)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _data.push(value);
        _cv.notify_one();
    }

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _cv.wait(lock, [this] { return !_data.empty(); });
        value = _data.front();
        _data.pop();
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_data.empty()) {
            return false;
        }
        value = _data.front();
        _data.pop();
        return true;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(_mtx);
        return _data.empty();
    }

private:
    threadsafe_queue& operator=(const threadsafe_queue&) = delete;

    std::queue<T> _data;
    mutable std::mutex _mtx;
    std::condition_variable _cv;
};
