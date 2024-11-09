#pragma once

#include <utility>
#include <mutex>
#include <shared_mutex>
#include <algorithm>
#include <memory>
#include <concepts>

namespace LCNS::ThreadSafe
{
    template <typename Value>
    class List1
    {
public:

    template<std::predicate Predicate>
    void remove_if(Predicate pred)
    {
        Node* current = &_head;

        std::unique_lock head_lock(_head._mutex);

        while (const Node* next = current->_next.get())
        {
            std::unique_lock next_lock(next->_mutex);

            if (pred(*next->_data))
            {
                std::unique_ptr<Node> old_next = std::move(current->next);
                current->next = std::move(next->next);
                next_lock.unlock();
            }
            else
            {
                head_lock.unlock();
                current=next;
                head_lock=std::move(next_lock);
            }
        }
    }

    private:
        struct Node
        {
            std::mutex _mutex;
            std::shared_ptr<Value> _data;
            std::unique_ptr<Value> _next;

            Node() = default;

            Node(const Value& value)
            :_data(std::make_shared<Value>(value))
            {}
        };

        Node _head;
    };

}  // namespace LCNS::ThreadSafe
