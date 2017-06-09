#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi {

    template<typename KeyType, typename ValueType>
    class TreeMap {
    public:
        using key_type = KeyType;
        using mapped_type = ValueType;
        using value_type = std::pair<const key_type, mapped_type>;
        using size_type = std::size_t;
        using reference = value_type &;
        using const_reference = const value_type &;

        class ConstIterator;

        class Iterator;

        using iterator = Iterator;
        using const_iterator = ConstIterator;
        typedef struct node {
            value_type pair;
            node *p;
            node *left;
            node *right;

            node(value_type A) : pair(A) {
                left = right = nullptr;
                p = nullptr;
            }

            ~node() {
                left = right = nullptr;
                p = nullptr;
            }
        } *nodeptr;

        nodeptr root;

        TreeMap() {
            root = nullptr;
        }

        TreeMap(std::initializer_list<value_type> list) : TreeMap() {
            for (auto it = list.begin(); it != (list.end()); ++it)
                insert(*it);
        }

        TreeMap(const TreeMap &other) : TreeMap() {
            preOrder(other.root);
        }

        TreeMap(TreeMap &&other) : TreeMap() {
            *this = std::move(other);
        }

        TreeMap &operator=(const TreeMap &other) {
            if (this == &other)
                return *this;
            for (auto it = begin(); it != end(); ++it)
                remove(it);
            root = nullptr;

            preOrder(other.root);

            return *this;
        }

        TreeMap &operator=(TreeMap &&other)
        {
            if(this != &other) {
                for (auto it = begin(); it != end(); ++it)
                    remove(it);
            }

            root = other.root;
            other.root = nullptr;

            return *this;
        }

        ~TreeMap() {
           erase(root);
           delete root;
        }

        void erase(nodeptr n){
            if(n == nullptr)
                return;
            erase(n->left);
            erase(n->right);
            if(n != root)
                delete n;
        }

        bool isEmpty() const {
            return root == nullptr;
        }

        mapped_type &operator[](const key_type &key) {
            const_iterator cit = this->find(key);
            if (cit != this->end()) {
                return cit.currNode->pair.second; 
            } else //cit == this->end()
            {
                value_type pair = std::make_pair(key, mapped_type());
                return insert(pair)->pair.second;
            }
        }

        const mapped_type &valueOf(const key_type &key) const {
            const_iterator cit = this->find(key);
            if (cit != this->end())
                return cit.currNode->pair.second;
            else
                throw std::out_of_range("Cannot valueof when item of given key does not exist");
        }

        mapped_type &valueOf(const key_type &key) {
            const_iterator cit = this->find(key);
            if (cit != this->end())
                return cit.currNode->pair.second;
            else
                throw std::out_of_range("Cannot valueof when item of given key does not exist");
        }

        const_iterator find(const key_type &key) const {
            nodeptr n = root;

            while (n != nullptr && n->pair.first != key) {
                if (key < n->pair.first)
                    n = n->left;
                else
                    n = n->right;
            }

            if (n == nullptr || n->pair.first != key) // ugly, yet saves a lotta thinking
                return cend();
            else
                return ConstIterator(root, n);
        }

        iterator find(const key_type &key) {
            nodeptr n = root;

            while (n != nullptr && n->pair.first != key) {
                if (key < n->pair.first)
                    n = n->left;
                else
                    n = n->right;
            }
            if (n == nullptr || n->pair.first != key) // ugly, yet saves a lotta thinking
                return end();
            else
                return Iterator(root, n);
        }

        void remove(const key_type &key) {
            remove(find(key));
        }

        void remove(const const_iterator &it) {
            nodeptr y, x;

            if (it.currNode == nullptr)
                throw std::out_of_range("Cannot remove a nonitem");
            if (it.currNode->left == nullptr || it.currNode->right == nullptr)
                y = it.currNode;
            else
                y = treeSucessor(it.currNode);

            if (y->left != nullptr)
                x = y->left;
            else
                x = y->right;

            if (x != nullptr)
                x->p = y->p;
            if (y->p == nullptr)
                root = x;
            else if (y == y->p->left)
                y->p->left = x;
            else
                y->p->right = x;
            delete (it.currNode);
        }

        size_type getSize() const {
            return (size_type) count(root);
        }

        bool operator==(const TreeMap &other) const {
            for (auto it = begin(); it != end(); ++it) {
                auto itO = other.find(it->first);
                if (itO == other.end() || itO->second != it->second)
                    return false;
            }
            return true;
        }

        bool operator!=(const TreeMap &other) const {
            return !(*this == other);
        }

        iterator begin() {
            return Iterator(root, treeMinimum(root));
        }

        iterator end() {
            return Iterator(root, nullptr);
        }

        const_iterator cbegin() const {
            return ConstIterator(root, treeMinimum(root));
        }

        const_iterator cend() const {
            return ConstIterator(root, nullptr);
        }

        const_iterator begin() const {
            return cbegin();
        }

        const_iterator end() const {
            return cend();
        }

    private:
        void preOrder(nodeptr n) {
            if (n == nullptr)
                return;
            insert(n->pair);
            preOrder(n->left);
            preOrder(n->right);
        }


        nodeptr insert(value_type pair) {

            nodeptr z = new node(pair);
            nodeptr y = nullptr;
            nodeptr x = this->root;

            while (x != nullptr) {
                y = x;
                if (z->pair.first < x->pair.first)
                    x = x->left;
                else
                    x = x->right;
            }

            z->p = y;
            if (y == nullptr)
                this->root = z;
            else if (z->pair.first < y->pair.first)
                y->left = z;
            else
                y->right = z;

            return z;
        }

        static nodeptr treeMinimum(nodeptr n) {
            if (n == nullptr)
                return nullptr;
            while (n->left != nullptr)
                n = n->left;
            return n;
        }

        static nodeptr treeMaximum(nodeptr n) {
            if (n == nullptr)
                return nullptr;
            while (n->right != nullptr)
                n = n->right;
            return n;
        }

        static nodeptr treeSucessor(nodeptr n) {
            if (n->right != nullptr)
                return treeMinimum(n->right);
            auto y = n->p;
            while (y != nullptr && n == y->right) {
                n = y;
                y = y->p;
            }
            return y;
        }

        static nodeptr treePredecessor(nodeptr n) {
            if (n->left != nullptr)
                return treeMaximum(n->left);
            auto y = n->p;
            while (y != nullptr && n == y->left) {
                n = y;
                y = y->p;
            }
            return y;
        }

        int count(nodeptr r) const {
            if (r == nullptr)
                return 0;
            else
                return (count(r->left) + count(r->right)) + 1;
        }

    };

    template<typename KeyType, typename ValueType>
    class TreeMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename TreeMap::const_reference;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename TreeMap::value_type;
        using pointer = const typename TreeMap::value_type *;

        nodeptr currNode;
        nodeptr rooot;

        explicit ConstIterator(nodeptr r, nodeptr n = nullptr) {
            rooot = r;
            currNode = n;
        }

        ConstIterator(const ConstIterator &other) {
            currNode = other.currNode;
        }

        ConstIterator &operator++() {
            if (currNode == nullptr)
                throw std::out_of_range("Cannot increment when iterator points to null");
            currNode = treeSucessor(currNode);
            return *this;
        }

        ConstIterator operator++(int) {
            auto result = this;
            ++(*this);
            return *result;
        }

        ConstIterator &operator--() {
            if (rooot == nullptr)
                throw std::out_of_range("Cannot decrement when tree is emptee");
            if (currNode == treeMinimum(rooot))
                throw std::out_of_range("Cannot decrement when pointing to begin()");
            if (currNode == nullptr)
                currNode = treeMaximum(rooot);
            else
                currNode = treePredecessor(currNode);
            return *this;
        }

        ConstIterator operator--(int) {
            auto result = this;
            --(*this);
            return *result;
        }

        reference operator*() const {
            if (currNode == nullptr)
                throw std::out_of_range("Cannot * on end iterator");
            return currNode->pair;
        }

        pointer operator->() const {
            return &(this->operator*());
        }

        bool operator==(const ConstIterator &other) const {
            return this->currNode == other.currNode;
        }

        bool operator!=(const ConstIterator &other) const {
            return !(*this == other);
        }
    };

    template<typename KeyType, typename ValueType>
    class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename TreeMap::reference;
        using pointer = typename TreeMap::value_type *;

        explicit Iterator(nodeptr r, nodeptr n) : ConstIterator(r, n) {}

        Iterator(const ConstIterator &other)
                : ConstIterator(other) {}

        Iterator &operator++() {
            ConstIterator::operator++();
            return *this;
        }

        Iterator operator++(int) {
            auto result = *this;
            ConstIterator::operator++();
            return result;
        }

        Iterator &operator--() {
            ConstIterator::operator--();
            return *this;
        }

        Iterator operator--(int) {
            auto result = *this;
            ConstIterator::operator--();
            return result;
        }

        pointer operator->() const {
            return &this->operator*();
        }

        reference operator*() const {
            // ugly cast, yet reduces code duplication.
            return const_cast<reference>(ConstIterator::operator*());
        }
    };

}

#endif /* AISDI_MAPS_MAP_H */
