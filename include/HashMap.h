#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include <list>
#include <utility>

class HashMap
{
private:
    static const int TABLE_SIZE = 10;

    // Each bucket stores key-value pairs.
    std::list<std::pair<int, std::string>> table[TABLE_SIZE];

    // Converts a key into a bucket index.
    int hashFunction(int key);

public:
    void insert(int key, std::string value);
    std::string search(int key);
    void remove(int key);
    void display();
};

#endif