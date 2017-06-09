#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H


class initializer_list;

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi {

    template<typename KeyType, typename ValueType>
    class HashMap {
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
            value_type Pair;
            node *next, *prev;

            node(value_type A) : Pair(A), next(nullptr), prev(nullptr) {}

            ~node() {
                next = prev = nullptr;
            }
        } *nodeptr;

        static const size_type ARRAYLENGTH = 1000;
        nodeptr head;
        nodeptr tail;

        nodeptr *array;
        size_type number;

        HashMap() {
            array = new nodeptr[ARRAYLENGTH];
            head = tail = nullptr;
            number = 0;
            for (size_type i = 0; i < ARRAYLENGTH; ++i) {
                array[i] = nullptr;
            }
        }

        HashMap(std::initializer_list<value_type> list) : HashMap() {
            for (auto it = list.begin(); it != list.end(); ++it)
                (*this)[it->first] = it->second;
        }

        HashMap(const HashMap &other) : HashMap() {
            *this = other;
        }

        HashMap(HashMap &&other) : HashMap() {
            *this = std::move(other);
        }

        HashMap &operator=(const HashMap &other) {
            if (this != &other) {
                if (number) {
                    for (size_type i = 0; i < ARRAYLENGTH; ++i) {
                        delete array[i];
                        array[i] = nullptr;
                    }
                    number = 0;
                }
                for (auto it = other.begin(); it != other.end(); ++it)
                    (*this)[it->first] = (*it).second;
            }
            return *this;
        }

        HashMap &operator=(HashMap &&other) {
            if (this != &other) {
                if (number) {
                    for (size_type i = 0; i < ARRAYLENGTH; ++i) {
                        delete array[i];
                        array[i] = nullptr;
                    }
                    number = 0;
                }

                auto help = array;

                array = other.array;
                number = other.number;

                other.array = help;
                other.number = 0;
            }
            return *this;
        }

        void erase(){
            for (size_type i = 0; i < ARRAYLENGTH; ++i) {
                        nodeptr n = array[i];
                        while(n != nullptr){
                            auto nprev = n;
                            n = n->next;
                            delete nprev;
                        }

                        delete array[i];
                        array[i] = nullptr;
                    }
        }

        ~HashMap() {
            if (number) {
                    erase();
                    number = 0;
                }
                delete[] array;
        }

        bool isEmpty() const {
            return number == 0;
        }

        mapped_type &operator[](const key_type &key) {
            size_type hashedIndex = hashingFunc(key);
            nodeptr n = array[hashedIndex];
            value_type pair = std::make_pair(key, mapped_type{});

            if (n == nullptr) {
                array[hashedIndex] = new node(pair);
                n = array[hashedIndex];
                ++number;
            } else if (n->Pair.first != key) {
                while (n->next != nullptr) {
                n = n->next;
            }
            ++number;
            n->next = new node(pair);
            }
            return n->Pair.second;
        }

        const mapped_type &valueOf(const key_type &key) const {
            nodeptr n = findNode(key);
            if (n == nullptr)
                throw std::out_of_range("Cannot valueof when pointing to end");
            return n->Pair.second;
        }

        mapped_type &valueOf(const key_type &key) {
            nodeptr n = findNode(key);
            if (n == nullptr)
                throw std::out_of_range("Cannot valueof when pointing to end");
            return n->Pair.second;
        }

        const_iterator find(const key_type &key) const {
            return const_iterator(this, findNode(key), hashingFunc(key));
        }

        iterator find(const key_type &key) {
            return iterator(this, findNode(key), hashingFunc(key));
        }

        void remove(const key_type &key) {
            remove(find(key));
        }

        void remove(const const_iterator &it) {
            if (this != it.hshMap || it == end())
                throw std::out_of_range("Cannot remove end");
            auto n = it.currNode;
            if (n->prev == nullptr)
                array[hashingFunc(it->first)] = n->next;
            else
                n->prev->next = n->next;

            if (n->next != nullptr)
                n->next->prev = n->prev;

            n->next = nullptr;
            delete n;
            number--;
        }

        size_type getSize() const {
            return number;
        }

        bool operator==(const HashMap &other) const {
            if (number != other.number)
                return false;

            for (auto it = begin(), it2 = other.begin(); it != end(); ++it, ++it2) {
                if (*it != (*it2))
                    return false;
            }
            return true;
        }

        bool operator!=(const HashMap &other) const {
            return !(*this == other);
        }

        iterator begin() {
            auto x = findFirstNode();
            return iterator(this, x.first, x.second);
        }

        iterator end() {
            return iterator(this);
        }

        const_iterator cbegin() const {
            auto x = findFirstNode();
            return const_iterator(this, x.first, x.second);
        }

        const_iterator cend() const {
            return const_iterator(this);
        }

        const_iterator begin() const {
            return cbegin();
        }

        const_iterator end() const {
            return cend();
        }

    private:
        size_type hashingFunc(const key_type &key) const {
            return std::hash<key_type>()(key) % ARRAYLENGTH;
        }

        std::pair<nodeptr, size_type> findFirstNode() const {
            size_type index = 0;

            while (index < ARRAYLENGTH && array[index] == nullptr)
                ++index;

            nodeptr n = nullptr;
            if (index < ARRAYLENGTH)
                n = array[index];
            return std::make_pair(n, index);
        }

        nodeptr findNode(const key_type &key) const {
            nodeptr n = array[hashingFunc(key)];
            while (n != nullptr) {
                if (n->Pair.first == key) {
                    return n;
                }
                n = n->next;
            }
            return n;
        }
    };

    template<typename KeyType, typename ValueType>
    class HashMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename HashMap::const_reference;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename HashMap::value_type;
        using pointer = const typename HashMap::value_type *;


        const HashMap *hshMap;
        nodeptr currNode;
        size_type index;

    public:
        explicit ConstIterator(const HashMap *hshMap = nullptr, nodeptr n = nullptr, size_type i = 0) :
                hshMap(hshMap), currNode(n), index(i) {
            if (currNode == nullptr && hshMap != nullptr)
                this->index = hshMap->ARRAYLENGTH;
        }

        ConstIterator(const ConstIterator &other) : ConstIterator(other.hshMap, other.currNode, other.index) {}

        ConstIterator &operator++() {
            if (hshMap == nullptr || currNode == nullptr)
                throw std::out_of_range("Cannot increment when pointing to the end");
            else if (currNode->next != nullptr)
                currNode = currNode->next;
            else {
                ++index;
                while (index < hshMap->ARRAYLENGTH && hshMap->array[index] == nullptr)
                    index++;
                if (index >= hshMap->ARRAYLENGTH)
                    currNode = nullptr;
                else
                    currNode = hshMap->array[index];
            }
            return *this;
        }

        ConstIterator operator++(int) {
            auto result = *this;
            ConstIterator::operator++();
            return result;
        }

        ConstIterator &operator--() {
            if (hshMap == nullptr)
                throw std::out_of_range("Cannot decrement");
            else if (currNode == nullptr || currNode == hshMap->array[index]) {
                index--;
                while (index > 0 && hshMap->array[index] == nullptr)
                    index--;

                if (index == 0 && hshMap->array[index] == nullptr)
                    throw std::out_of_range("Cannot decrement");

                currNode = hshMap->array[index];
                while (currNode->next != nullptr)
                    currNode = currNode->next;
            } else {
                currNode = currNode->prev;
            }

            return *this;
        }

        ConstIterator operator--(int) {
            auto result = *this;
            ConstIterator::operator--();
            return result;
        }

        reference operator*() const {
            if (currNode == nullptr)
                throw std::out_of_range("Cannot dereference when pointing to the nullptr");
            return currNode->Pair;
        }

        pointer operator->() const {
            return &this->operator*();
        }

        bool operator==(const ConstIterator &other) const {
            return hshMap == other.hshMap && currNode == other.currNode && index == other.index;
        }

        bool operator!=(const ConstIterator &other) const {
            return !(*this == other);
        }
    };

    template<typename KeyType, typename ValueType>
    class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename HashMap::reference;
        using pointer = typename HashMap::value_type *;

        explicit Iterator(HashMap *hshMap = nullptr, nodeptr n = nullptr, size_type i = 0) :
                ConstIterator(hshMap, n, i) {}

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

#endif /* AISDI_MAPS_HASHMAP_H */
