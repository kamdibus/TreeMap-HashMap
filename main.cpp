
#include <cstddef>
#include <cstdlib>
#include <string>

#include "TreeMap.h"
#include "HashMap.h"
#include <iostream>
#include <chrono>
#include <ctime>

namespace {

    template<typename K, typename V>
    using TreeMap = aisdi::TreeMap<K, V>;
    template<typename K, typename V>
    using HashMap = aisdi::HashMap<K, V>;

    void performTestsOnTree()
    {
        TreeMap<size_t, char> tree;
        auto startL = std::chrono::system_clock::now();
        for (std::size_t i = 0; i < 10000; ++i)
            tree[i%124] = (char)(i % 92 + 33);
        auto endL = std::chrono::system_clock::now();
        auto elapsedL =
                std::chrono::duration_cast<std::chrono::milliseconds>(endL - startL);
        std::cout << "Inserting 100k characters to tree lasted " << elapsedL.count() << " ms\n";

        startL = std::chrono::system_clock::now();
        for (std::size_t i = 0; i < 10000; ++i)
            tree.find(i%124);
        endL = std::chrono::system_clock::now();
        elapsedL =
                std::chrono::duration_cast<std::chrono::milliseconds>(endL - startL);
        std::cout << "Searching 100k by keys lasted " << elapsedL.count() << " ms for the Tree\n";
    }

    void performTestsOnHash()
    {
        HashMap<int, char> hash;
        auto startL = std::chrono::system_clock::now();
        for (int i = 0; i < 10000; ++i)
            hash[i] = (char)(i % 92 + 33);
        auto endL = std::chrono::system_clock::now();
        auto elapsedL =
                std::chrono::duration_cast<std::chrono::milliseconds>(endL - startL);
        std::cout << "Inserting 100k characters to hash lasted " << elapsedL.count() << " ms\n";

        startL = std::chrono::system_clock::now();
        for (int i = 0; i < 10000; ++i)
            hash.valueOf(i);
        endL = std::chrono::system_clock::now();
        elapsedL =
                std::chrono::duration_cast<std::chrono::milliseconds>(endL - startL);
        std::cout << "Searching 100k by keys lasted " << elapsedL.count() << " ms for the Hash\n";
    }


    void AreThereLeaks()
    {
        //std::cout << "ToDo";
    }

} // namespace

int main(int argc, char** argv)
{
    const std::size_t repeatCount = argc > 1 ? std::atoll(argv[1]) : 1;

    for (std::size_t i = 0; i < repeatCount; ++i)
        AreThereLeaks();
    if(repeatCount == 1)
    {
        performTestsOnHash();
        std::cout << std::endl;
        performTestsOnTree();
    }

    return 0;
}
