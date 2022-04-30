#ifndef __SKIPLIST_HPP__
#define __SKIPLIST_HPP__

#include "common.h"

template <typename K, typename V>
class SkipList
{
private:
    enum NodeType
    {
        Data,
        Guard
    };

    struct Node
    {
        K key;
        V value;
        NodeType node_type;
        // level
        weak_ptr<Node> prev;
        shared_ptr<Node> succ;
        // tower
        weak_ptr<Node> above;
        shared_ptr<Node> below;

        Node()
        {
            node_type = NodeType::Guard;
            // use default constructor of weak_ptr
            succ = below = nullptr;
        }

        Node(const K &k, const V &v)
        {
            key = k, value = v;
            node_type = NodeType::Data;
            // use default constructor of weak_ptr
            succ = below = nullptr;
        }

        bool equal(const K &k)
        {
            return node_type == NodeType::Data && key == k;
        }

        bool less(const K &k)
        {
            return node_type == NodeType::Data && key < k;
        }
    };

    u32 n_node;
    // NOTE:
    //  head = top left guard node
    //  tail = top right guard node
    shared_ptr<Node> head, tail;

private:
    // insert x after "prev" and above "below"
    //  prev --->   x   ---> prev->succ
    //  .... ---> below ---> ....
    void insert(
        shared_ptr<Node> x,
        shared_ptr<Node> prev,
        shared_ptr<Node> below = nullptr)
    {
        x->succ = prev->succ;
        prev->succ->prev = x;
        x->prev = prev;
        prev->succ = x;

        if (below != nullptr)
        {
            // NOTE: below->above is nullptr
            x->below = below;
            below->above = x;
        }
    }

    // returns: top node of key' (key' <= key)
    shared_ptr<Node> search(const K &key)
    {
        auto x = head;
        while (true)
        {
            while (x->succ->less(key) ||
                   x->succ->equal(key))
                x = x->succ;
            if (x->equal(key) ||
                x->below == nullptr)
                return x;
            x = x->below;
        }
    }

public:
    // DEBUG function
    void display()
    {
        for (auto s = head; s != nullptr; s = s->below)
        {
            for (auto x = s; x != nullptr; x = x->succ)
                if (x->node_type == NodeType::Data)
                    std::cout << x->key << ' ';
            std::cout << '\n';
        }
        std::cout << std::endl;
    }

public:
    SkipList()
    {
        n_node = 0;
        head = make_shared<Node>();
        tail = make_shared<Node>();
        head->succ = tail;
        tail->prev = head;
    }

    u32 size() { return n_node; }

    void clear()
    {
        head = tail = nullptr;

        n_node = 0;
        head = make_shared<Node>();
        tail = make_shared<Node>();
        head->succ = tail;
        tail->prev = head;
    }

    void insert(const K &key, const V &value)
    {
        auto x = search(key);
        if (x->equal(key))
        {
            x->value = value;
            return;
        }

        n_node++;
        while (x->below != nullptr)
            x = x->below; // go to bottom
        auto below = make_shared<Node>(key, value);
        insert(below, x);

        while (randomInt(0, 1))
        {
            while (x != head &&
                   x->above.expired())
                x = x->prev.lock();

            // x is head, need upgrade
            if (head == x)
            {
                auto t = make_shared<Node>();
                t->below = head;
                head->above = t, head = t;
                head->succ = make_shared<Node>();
                head->succ->below = tail;
                tail->above = head->succ;
                tail = head->succ;
            }

            x = x->above.lock();
            auto t = make_shared<Node>(key, value);
            insert(t, x, below);
            below = t;
        }
    }

    void remove(const K &key)
    {
        auto x = search(key);
        if (!x->equal(key))
            return;

        n_node--;
        while (x != nullptr)
        {
            x->prev.lock()->succ = x->succ;
            x->succ->prev = x->prev;
            x = x->below;
        }

        // remove top level (if empty)
        while (head->succ == tail &&
               head->below != nullptr)
        {
            head = head->below;
            tail = tail->below;
        }
    }

    pair<bool, V> at(const K &key)
    {
        auto result = search(key);
        return result->equal(key)
                   ? make_pair(true, result->value)
                   : make_pair(false, V());
    }
};

#endif