#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>

namespace LCNS::TS
{
    template <typename T>
    class Queue1
    {
    public:
        Queue1()               = default;
        Queue1(const Queue1&) = delete;

        const T& front() const
        {
            std::scoped_lock lk(_queue_mutex);
            return _queue.front();
        }

        const T& back() const
        {
            std::scoped_lock lk(_queue_mutex);
            return _queue.back();
        }

        void push_back(T item)
        {
            std::scoped_lock lk(_queue_mutex);
            _queue.push_back(std::forward<T>(item));

            std::unique_lock<std::mutex> ul(_wait_for_work_mutex);
            _wait_for_work.notify_one();
        }

        void push_front(T item)
        {
            std::scoped_lock lk(_queue_mutex);
            _queue.push_front(std::forward<T>(item));

            std::unique_lock<std::mutex> ul(_wait_for_work_mutex);
            _wait_for_work.notify_one();
        }

        bool is_empty() const
        {
            std::scoped_lock lk(_queue_mutex);
            return _queue.empty();
        }

        std::size_t size() const
        {
            std::scoped_lock lk(_queue_mutex);
            return _queue.size();
        }

        void clear()
        {
            std::scoped_lock lk(_queue_mutex);
            return _queue.clear();
        }

        T pop_front()
        {
            std::scoped_lock lk(_queue_mutex);
            const auto       t = _queue.front();
            _queue.pop_front();
            return t;
        }

        T pop_back()
        {
            std::scoped_lock lk(_queue_mutex);
            const auto       t = _queue.back();
            _queue.pop_back();
            return t;
        }

        void wait()
        {
            while (is_empty() && !_force_stop_waiting.load())
            {
                std::unique_lock<std::mutex> lock(_wait_for_work_mutex);
                _wait_for_work.wait(lock);
            }
        }

        void force_stop_waiting()
        {
            _force_stop_waiting.store(true);
            _wait_for_work.notify_all();
        }

    private:
        mutable std::mutex      _queue_mutex;
        std::mutex              _wait_for_work_mutex;
        std::deque<T>           _queue;
        std::condition_variable _wait_for_work;
        std::atomic<bool>       _force_stop_waiting = false;
    };
}