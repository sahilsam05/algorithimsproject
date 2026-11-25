#include "HashMap.h"

#include <algorithm>
#include <iostream>

HashMap::HashMap(std::size_t initialBuckets, bool autoResize)
    : table(initialBuckets == 0 ? 1 : initialBuckets), autoResize(autoResize)
{
}

std::size_t HashMap::hashFunction(int key) const
{
    
    return static_cast<unsigned int>(key) % table.size();
}

void HashMap::rehash(std::size_t newBucketCount)
{
    std::vector<std::list<std::pair<int, std::string>>> newTable(newBucketCount);

    for (auto& bucket : table)
    {
        for (auto& entry : bucket)
        {
            std::size_t index = static_cast<unsigned int>(entry.first) % newBucketCount;
            newTable[index].push_back(std::move(entry));
        }
    }

    table.swap(newTable);
}

void HashMap::insert(int key, const std::string& value)
{
    std::size_t index = hashFunction(key);

    // If the key already exists, update its value.
    for (auto& entry : table[index])
    {
        if (entry.first == key)
        {
            entry.second = value;
            return;
        }
    }

    table[index].push_back({ key, value });
    ++count;

    if (autoResize && loadFactor() > MAX_LOAD_FACTOR)
    {
        rehash(table.size() * 2);
    }
}

std::string HashMap::search(int key) const
{
    std::size_t index = hashFunction(key);

    for (const auto& entry : table[index])
    {
        if (entry.first == key)
        {
            return entry.second;
        }
    }

    return "Student not found";
}

bool HashMap::contains(int key) const
{
    std::size_t index = hashFunction(key);

    for (const auto& entry : table[index])
    {
        if (entry.first == key)
        {
            return true;
        }
    }

    return false;
}

bool HashMap::remove(int key)
{
    std::size_t index = hashFunction(key);

    for (auto it = table[index].begin(); it != table[index].end(); ++it)
    {
        if (it->first == key)
        {
            table[index].erase(it);
            --count;
            return true;
        }
    }

    return false;
}

void HashMap::display() const
{
    for (std::size_t i = 0; i < table.size(); i++)
    {
        std::cout << "Bucket " << i << ": ";

        for (const auto& entry : table[i])
        {
            std::cout << "[" << entry.first << ", "
                << entry.second << "] -> ";
        }

        std::cout << "NULL" << std::endl;
    }
}

std::size_t HashMap::size() const
{
    return count;
}

std::size_t HashMap::bucketCount() const
{
    return table.size();
}

double HashMap::loadFactor() const
{
    return static_cast<double>(count) / static_cast<double>(table.size());
}

std::size_t HashMap::longestBucket() const
{
    std::size_t longest = 0;
    for (const auto& bucket : table)
    {
        longest = std::max(longest, bucket.size());
    }
    return longest;
}
