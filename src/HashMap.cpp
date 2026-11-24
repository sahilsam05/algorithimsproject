#include "HashMap.h"

// Converts a student ID into a bucket index.
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