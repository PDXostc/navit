#ifndef CONCURRENT_QUEUE_HPP
#define CONCURRENT_QUEUE_HPP

#include <mutex>
#include <queue>
#include <condition_variable>

template <typename Data, typename Mutex = std::mutex>
class concurrent_queue {
private:
    std::queue<Data> _queue;
    mutable Mutex _mutex;
    std::condition_variable _condVar;

public:
    template <typename DataUR>
    void push(DataUR&& entry)
    {
        std::unique_lock<Mutex> guard{ _mutex };
        _queue.push(std::forward<DataUR>(entry));
        guard.unlock();
        _condVar.notify_one();
    }

    bool empty() const
    {
        std::lock_guard<Mutex> guard{ _mutex };
        return _queue.empty();
    }

    bool try_pop(Data& val)
    {
        std::lock_guard<Mutex> guard{ _mutex };
        if (_queue.empty()) {
            return false;
        }

        val = _queue.front();
        _queue.pop();
        return true;
    }

    void wait_and_pop(Data& popped_value)
    {
        std::unique_lock<Mutex> guard{ _mutex };
        while (_queue.empty()) {
            _condVar.wait(guard);
        }

        popped_value = _queue.front();
        _queue.pop();
    }
};

#endif // CONCURRENT_QUEUE_HPP
