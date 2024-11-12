#pragma once

#include <atomic>

namespace LCNS::ThreadSafe
{
    template <typename Data>
    class LockFreeStack
    {
    public:
        void push(const Data& data);
        void pop(Data& data);

    private:
        struct Node
        {
            Node(const Data& input_data)
            : data(input_data)
            {
            }

            Data  data;
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
    inline void LockFreeStack<Data>::pop(Data& data)
    {
        Node* old_head = _head.load();
        while(!_head.compare_exchange_weak(old_head, old_head->next));
        data = old_head->data;
    }

}  // namespace LCNS::ThreadSafe
