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
        template <typename U>
        struct Node
        {
            std::unique_ptr<U>       data;
            std::unique_ptr<Node<U>> next;
        };
        std::unique_ptr<Node<T>> _head;
        Node<T>*                 _tail = nullptr;
        mutable std::mutex       _mutex;
        std::condition_variable  _data_condition;
        std::size_t              _size = 0ul;
    };

    template <typename T>
    inline Queue2<T>::Queue2()
    : _head(std::make_unique<Node<T>>())
    , _tail(_head.get())
    {
    }

    template <typename T>
    inline void Queue2<T>::push(T item)
    {
        auto new_data = std::make_unique<T>(std::forward<T>(item));
        auto new_node = std::make_unique<Node<T>>();

        _tail->data   = std::move(new_data);

        Node<T>* new_tail = new_node.get();

        _tail->next = std::move(new_node);

        _tail = new_tail;

        ++_size;
    }

    template <typename T>
    inline bool Queue2<T>::try_pop(T& value)
    {
        if (_head.get() == _tail)
        {
            return false;
        }

        value = *_head->data;

        _head = std::move(_head->next);

        --_size;
        return true;
    }

    template <typename T>
    inline void Queue2<T>::wait_and_pop([[maybe_unused]] T& value)
    {
    }

    template <typename T>
    inline bool Queue2<T>::is_empty() const
    {
        return _head.get() == _tail;
    }

    template <typename T>
    inline std::size_t Queue2<T>::size() const
    {
        return _size;
    }

    template <typename T>
    inline void Queue2<T>::clear()
    {
        if (_size == 0ul)
        {
            return;
        }

        _head.reset();

        _head = std::make_shared<Node<T>>();
        _tail = _head.get();
        _size = 0ul;
    }

}  // namespace LCNS::ThreadSafe
