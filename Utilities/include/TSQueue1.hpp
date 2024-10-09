#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

namespace LCNS::ThreadSafe
{
    template <typename T>
    class Queue1
    {
    public:
        Queue1()                         = default;
        Queue1(const Queue1&)            = delete;
        Queue1(Queue1&&)                 = delete;
        Queue1& operator=(const Queue1&) = delete;
        Queue1& operator=(Queue1&&)      = delete;
        ~Queue1()                        = default;

        void push(T item);

        bool try_pop(T& value);

        void wait_and_pop(T& value);

        bool is_empty() const;

        std::size_t size() const;

        void clear();

    private:
        mutable std::mutex      _mutex;
        std::queue<T>           _queue;
        std::condition_variable _data_condition;
    };

    template <typename T>
    inline void Queue1<T>::push(T item)
    {
        std::scoped_lock lk(_mutex);
        _queue.push(std::forward<T>(item));
        _data_condition.notify_one();
    }

    template <typename T>
    inline bool Queue1<T>::try_pop(T& value)
    {
        std::scoped_lock lk(_mutex);

        if (_queue.empty())
        {
            return false;
        }

        value = std::forward<T>(_queue.front());
        _queue.pop();

        return true;
    }

    template <typename T>
    inline void Queue1<T>::wait_and_pop(T& value)
    {
        std::unique_lock lk(_mutex);
        _data_condition.wait(lk, [this]() { return !_queue.empty(); });

        value = std::forward<T>(_queue.front());
        _queue.pop();
    }

    template <typename T>
    inline bool Queue1<T>::is_empty() const
    {
        std::scoped_lock lk(_mutex);
        return _queue.empty();
    }

    template <typename T>
    inline std::size_t Queue1<T>::size() const
    {
        std::scoped_lock lk(_mutex);
        return _queue.size();
    }

    template <typename T>
    inline void Queue1<T>::clear()
    {
        std::scoped_lock lk(_mutex);

        while (!_queue.empty())
        {
            _queue.pop();
        }
    }
}