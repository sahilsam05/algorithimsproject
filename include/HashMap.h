#ifndef HASHMAP_H
#define HASHMAP_H

#include <cstddef>
#include <list>
#include <string>
#include <utility>
#include <vector>


class HashMap
{
private:
    
    std::vector<std::list<std::pair<int, std::string>>> table;

    std::size_t count = 0;      
    bool autoResize;            

    
    static constexpr double MAX_LOAD_FACTOR = 1.0;

    std::size_t hashFunction(int key) const;

    void rehash(std::size_t newBucketCount);

public:
    
    explicit HashMap(std::size_t initialBuckets = 10, bool autoResize = true);

    void insert(int key, const std::string& value);  
    std::string search(int key) const;               
    bool contains(int key) const;
    bool remove(int key);                            
    void display() const;

    std::size_t size() const;
    std::size_t bucketCount() const;
    double loadFactor() const;
    std::size_t longestBucket() const;
};

#endif
