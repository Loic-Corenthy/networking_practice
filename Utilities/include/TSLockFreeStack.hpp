#pragma once

#include <atomic>
#include <memory>

namespace LCNS::ThreadSafe
{
    template <typename Data>
    class LockFreeStack
    {
    public:
        void push(const Data& data);
        std::shared_ptr<Data> pop();

    private:
        struct Node
        {
            Node(const Data& input_data)
            : data(std::make_shared<Data>(input_data))
            {
            }

            std::shared_ptr<Data>  data;
            Node* next;
        };

        std::atomic<Node*> _head;
    };

    template <typename Data>
    inline void LockFreeStack<Data>::push(const Data& data)
    {
        Node* new_node = new Node(data);
        new_node->next = _head.load();
        while (!_head.compare_exchange_weak(new_node->next, new_node));
    }

    template <typename Data>
    inline std::shared_ptr<Data> LockFreeStack<Data>::pop()
    {
        Node* old_head = _head.load();
        while(old_head && !_head.compare_exchange_weak(old_head, old_head->next));

        return old_head ? old_head->data : nullptr;
    }

}  // namespace LCNS::ThreadSafe
