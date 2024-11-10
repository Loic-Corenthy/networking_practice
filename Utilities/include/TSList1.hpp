#pragma once

#include <utility>
#include <mutex>
#include <shared_mutex>
#include <algorithm>
#include <memory>
#include <concepts>

namespace LCNS::ThreadSafe
{
    template <typename Func, typename T>
    concept InvocableWithBoth = std::invocable<Func, T> || std::invocable<Func, T&>;

    template <typename Value>
    class List1
    {
    public:
        List1() = default;

        List1(const List1&)            = delete;
        List1(List1&&)                 = delete;
        List1& operator=(const List1&) = delete;
        List1& operator=(List1&&)      = delete;
        ~List1();

        void push_front(const Value& value);

        template <InvocableWithBoth<Value> Function>
        void for_each(Function func);

        // template<std::predicate<Value>

        template <std::predicate<Value> Predicate>
        void remove_if(Predicate pred);

    private:
        struct Node
        {
            std::mutex             mutex;
            std::shared_ptr<Value> data;
            std::unique_ptr<Node>  next;

            Node() = default;

            Node(const Value& value)
            : data(std::make_shared<Value>(value))
            {
            }
        };

        Node _head;
    };


    template <typename Value>
    inline List1<Value>::~List1()
    {
        remove_if([]([[maybe_unused]] const Value& v) { return true; });
    }

    template <typename Value>
    inline void List1<Value>::push_front(const Value& value)
    {
        auto new_node = std::make_unique<Node>(value);

        std::scoped_lock lock(_head.mutex);
        new_node->next = std::move(_head.next);
        _head.next    = std::move(new_node);
    }

    template <typename Value>
    template <InvocableWithBoth<Value> Function>
    inline void List1<Value>::for_each(Function func)
    {
        Node* current = &_head;

        std::unique_lock lock(current->mutex);

        while (Node* next_node = current->next.get())
        {
            std::unique_lock next_lock(next_node->mutex);
            lock.unlock();
            func(*next_node->data);
            current = next_node;
            lock    = std::move(next_lock);
        }
    }

    template <typename Value>
    template <std::predicate<Value> Predicate>
    void List1<Value>::remove_if(Predicate pred)
    {
        Node* current_node = &_head;

        std::unique_lock lock(_head.mutex);

        while (Node* const next_node = current_node->next.get())
        {
            std::unique_lock next_lock(next_node->mutex);

            if (pred(*next_node->data))
            {
                std::unique_ptr<Node> old_next = std::move(current_node->next);
                current_node->next             = std::move(next_node->next);
                next_lock.unlock();
            }
            else
            {
                lock.unlock();
                current_node = next_node;
                lock         = std::move(next_lock);
            }
        }
    }


}  // namespace LCNS::ThreadSafe
