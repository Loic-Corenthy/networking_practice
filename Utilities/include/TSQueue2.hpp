#pragma once

#include <mutex>
#include <condition_variable>

namespace LCNS::ThreadSafe
{
    template <typename T>
    class Queue2
    {
    public:
        Queue2();
        Queue2(const Queue2&)            = delete;
        Queue2(Queue2&&)                 = delete;
        Queue2& operator=(const Queue2&) = delete;
        Queue2& operator=(Queue2&&)      = delete;
        ~Queue2() { _head.reset(); }

        void push(T item);

        bool try_pop(T& value);

        void wait_and_pop(T& value);

        bool is_empty() const;

        std::size_t size() const;

        void clear();

    private:
        struct Node
        {
            std::unique_ptr<T>    data;
            std::unique_ptr<Node> next;
        };

    private:
        Node* get_tail() const;

        std::unique_ptr<Node> pop_head();

        std::unique_lock<std::mutex> wait_for_data() const;

        std::unique_ptr<Node> wait_pop_head();

        std::unique_ptr<Node> wait_pop_head(T& value);

        std::unique_ptr<Node> try_pop_head();

        std::unique_ptr<Node> try_pop_head(T& value);

    private:
        std::unique_ptr<Node>   _head;
        Node*                   _tail = nullptr;
        mutable std::mutex      _head_mutex;
        mutable std::mutex      _tail_mutex;
        std::condition_variable _data_condition;
        std::size_t             _size = 0ul;
    };

    template <typename T>
    inline Queue2<T>::Queue2()
    : _head(std::make_unique<Node>())
    , _tail(_head.get())
    {
    }

    template <typename T>
    inline void Queue2<T>::push(T item)
    {
        auto new_data = std::make_unique<T>(std::forward<T>(item));
        auto new_node = std::make_unique<Node>();

        {
            std::scoped_lock tail_lock(_tail_mutex);

            Node* new_tail = new_node.get();

            _tail->data = std::move(new_data);
            _tail->next = std::move(new_node);

            _tail = new_tail;

            ++_size;
        }

        _data_condition.notify_one();
    }

    template <typename T>
    inline bool Queue2<T>::try_pop(T& value)
    {
        const auto old_head = try_pop_head(value);

        if (!old_head)
        {
            return false;
        }

        --_size;
        return true;
    }

    template <typename T>
    inline void Queue2<T>::wait_and_pop(T& value)
    {
        wait_pop_head(value);
    }

    template <typename T>
    inline bool Queue2<T>::is_empty() const
    {
        std::scoped_lock head_lock(_head_mutex);
        return _head.get() == get_tail();
    }

    template <typename T>
    inline std::size_t Queue2<T>::size() const
    {
        std::scoped_lock both_lock(_head_mutex, _tail_mutex);
        return _size;
    }

    template <typename T>
    inline void Queue2<T>::clear()
    {
        std::scoped_lock both_lock(_head_mutex, _tail_mutex);

        if (_size == 0ul)
        {
            return;
        }

        _head.reset();

        _head = std::make_unique<Node>();
        _tail = _head.get();
        _size = 0ul;
    }

    template <typename T>
    inline Queue2<T>::Node* Queue2<T>::get_tail() const
    {
        std::scoped_lock tail_lock(_tail_mutex);
        return _tail;
    }

    template <typename T>
    inline std::unique_ptr<typename Queue2<T>::Node> Queue2<T>::pop_head()
    {
        std::unique_ptr<Node> old_head = std::move(_head);

        _head = std::move(old_head->next);

        return old_head;
    }

    template <typename T>
    inline std::unique_lock<std::mutex> Queue2<T>::wait_for_data() const
    {
        std::unique_lock head_lock(_head_mutex);

        _data_condition.wait(head_lock, [this]() { return _head.get() != get_tail(); });

        return head_lock;
    }

    template <typename T>
    inline std::unique_ptr<typename Queue2<T>::Node> Queue2<T>::wait_pop_head()
    {
        std::unique_lock head_lock(wait_for_data());
        return pop_head();
    }

    template <typename T>
    inline std::unique_ptr<typename Queue2<T>::Node> Queue2<T>::wait_pop_head(T& value)
    {
        std::unique_lock head_lock(wait_for_data());
        value = std::forward<T>(*_head->data);
        return pop_head();
    }

    template <typename T>
    inline std::unique_ptr<typename Queue2<T>::Node> Queue2<T>::try_pop_head()
    {
        std::scoped_lock head_lock(_head_mutex);

        if (_head.get() == get_tail())
        {
            return nullptr;
        }

        return pop_head();
    }

    template <typename T>
    inline std::unique_ptr<typename Queue2<T>::Node> Queue2<T>::try_pop_head(T& value)
    {
        std::scoped_lock head_lock(_head_mutex);

        if (_head.get() == get_tail())
        {
            return nullptr;
        }

        value = std::forward<T>(*_head->data);

        return pop_head();
    }

}  // namespace LCNS::ThreadSafe
