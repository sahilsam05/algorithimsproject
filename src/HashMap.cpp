#include "HashMap.h"
#include <iostream>

int HashMap::hashFunction(int key)
{
    return key % TABLE_SIZE;
}

void HashMap::insert(int key, std::string value)
{
    int index = hashFunction(key);

    for (auto& entry : table[index])
    {
        if (entry.first == key)
        {
            entry.second = value;
            return;
        }
    }

    table[index].push_back({ key, value });
}

std::string HashMap::search(int key)
{
    int index = hashFunction(key);

    for (auto& entry : table[index])
    {
        if (entry.first == key)
        {
            return entry.second;
        }
    }

    return "Student not found";
}

void HashMap::remove(int key)
{
    int index = hashFunction(key);

    for (auto it = table[index].begin(); it != table[index].end(); ++it)
    {
        if (it->first == key)
        {
            table[index].erase(it);
            return;
        }
    }
}

void HashMap::display()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        std::cout << "Bucket " << i << ": ";

        for (auto& entry : table[i])
        {
            std::cout << "[" << entry.first << ", "
                << entry.second << "] -> ";
        }

        std::cout << "NULL" << std::endl;
    }
}