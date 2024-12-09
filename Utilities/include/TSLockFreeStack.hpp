#pragma once

#include <atomic>
#include <memory>

namespace LCNS::ThreadSafe
{
    template <typename Data>
    class LockFreeStack
    {
    public:
        void                  push(const Data& data);
        std::shared_ptr<Data> pop();

    private:
        struct Node
        {
            Node(const Data& input_data)
            : data(std::make_shared<Data>(input_data))
            {
            }

            std::shared_ptr<Data> data;
            Node*                 next;
        };

        void try_reclaim(Node* old_head);
        void chain_pending_nodes(Node* nodes);
        void chain_pending_nodes(Node* first, Node* last);
        void chain_pending_node(Node* node);
        void delete_nodes(Node* nodes);

    private:
        std::atomic<Node*>        _head = nullptr;
        std::atomic<unsigned int> _threads_in_pop;
        std::atomic<Node*>        _to_be_deleted;
    };

    template <typename Data>
    inline void LockFreeStack<Data>::push(const Data& data)
    {
        Node* new_node = new Node(data);
        new_node->next = _head.load();
        while (!_head.compare_exchange_weak(new_node->next, new_node)) {}
    }

    template <typename Data>
    inline std::shared_ptr<Data> LockFreeStack<Data>::pop()
    {
        std::atomic<void*>& hazard_pointer = get_hazard_pointer_for_current_thread();

        Node* old_head = _head.load();
        Node* tmp = nullptr;

        do
        {
            tmp = old_head;
            hazard_pointer.store(old_head);
            old_head = _head.load();
        } while (old_head != tmp);
        

        std::shared_ptr<Data> result;

        return result;
    }

    template <typename Data>
    inline void LockFreeStack<Data>::try_reclaim(Node* old_head)
    {
        if (_threads_in_pop.load() == 1)
        {
            Node* nodes_to_delete = _to_be_deleted.exchange(nullptr);

            if (!--_threads_in_pop)
            {
                delete_nodes(nodes_to_delete);
            }
            else if (nodes_to_delete)
            {
                chain_pending_nodes(nodes_to_delete);
            }

            delete old_head;
        }
        else
        {
            chain_pending_node(old_head);
            --_threads_in_pop;
        }
    }

    template <typename Data>
    inline void LockFreeStack<Data>::chain_pending_nodes(Node* nodes)
    {
        Node* last = nodes;

        while (Node* const next = last->next)
        {
            last = next;
        }

        chain_pending_nodes(nodes, last);
    }

    template <typename Data>
    inline void LockFreeStack<Data>::chain_pending_nodes(Node* first, Node* last)
    {
        last->next = _to_be_deleted;
        while (!_to_be_deleted.compare_exchange_weak(last->next, first)) {}
    }

    template <typename Data>
    inline void LockFreeStack<Data>::chain_pending_node(Node* node)
    {
        chain_pending_nodes(node, node);
    }

    template <typename Data>
    inline void LockFreeStack<Data>::delete_nodes(Node* nodes)
    {
        while (nodes)
        {
            Node* next = nodes->next;
            delete nodes;
            nodes = next;
        }
    }

}  // namespace LCNS::ThreadSafe
