#pragma once

#include <mutex>
#include <condition_variable>

namespace LCNS::ThreadSafe
{
    template <typename T>
    class Queue2
    {
    public:
        Queue2()                         = default;
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
            Node(U init_data)
            : data(std::forward<U>(init_data))
            {
            }

            U                        data;
            std::unique_ptr<Node<U>> next;
        };
        std::unique_ptr<Node<T>> _head;
        Node<T>*                 _tail = nullptr;
        mutable std::mutex       _mutex;
        std::condition_variable  _data_condition;
        std::size_t              _size = 0ul;
    };

    template <typename T>
    inline void Queue2<T>::push(T item)
    {
        std::unique_ptr<Node<T>>  new_node(new Node<T>(std::forward<T>(item)));
        std::scoped_lock lk(_mutex);
        Node<T>*         new_tail = new_node.get();

        if (_tail)
        {
            _tail->next = std::move(new_node);
        }
        else
        {
            _head = std::move(new_node);
        }
        _tail = new_tail;

        ++_size;
        _data_condition.notify_one();
    }

    template <typename T>
    inline bool Queue2<T>::try_pop(T& value)
    {
        std::scoped_lock lk(_mutex);
        if (!_head)
        {
            return false;
        }

        value = std::forward<T>(_head->data);

        _head = std::move(_head->next);

        if (!_head)
        {
            _tail = nullptr;
        }

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
        std::scoped_lock lk(_mutex);
        return !_head;
    }

    template <typename T>
    inline std::size_t Queue2<T>::size() const
    {
        std::scoped_lock lk(_mutex);
        return _size;
    }

    template <typename T>
    inline void Queue2<T>::clear()
    {
        std::scoped_lock lk(_mutex);

        _size = 0ul;
    }

}  // namespace LCNS::ThreadSafe
