#pragma once

#include "NetCommon.hpp"

#include <deque>
#include <mutex>

namespace LCNS::Net
{
    template <typename T>
    class TSQueue
    {
    public:
        TSQueue()               = default;
        TSQueue(const TSQueue&) = delete;

        const T& front() const
        {
            std::scoped_lock lk(_mutex);
            return _queue.front();
        }

        const T& back() const
        {
            std::scoped_lock lk(_mutex);
            return _queue.back();
        }

        void push_back(T item)
        {
            std::scoped_lock lk(_mutex);
            _queue.push_back(std::forward<T>(item));
        }

        void push_front(T item)
        {
            std::scoped_lock lk(_mutex);
            _queue.push_front(std::forward<T>(item));
        }

        bool is_empty() const
        {
            std::scoped_lock lk(_mutex);
            return _queue.empty();
        }

        std::size_t size() const
        {
            std::scoped_lock lk(_mutex);
            return _queue.size();
        }

        void clear()
        {
            std::scoped_lock lk(_mutex);
            return _queue.clear();
        }

        T pop_front()
        {
            std::scoped_lock lk(_mutex);
            const auto       t = _queue.front();
            _queue.pop_front();
            return t;
        }

        T pop_back()
        {
            std::scoped_lock lk(_mutex);
            const auto       t = _queue.back();
            _queue.pop_back();
            return t;
        }

    private:
        mutable std::mutex _mutex;
        std::deque<T>      _queue;
    };

}  // namespace LCNS::Net
