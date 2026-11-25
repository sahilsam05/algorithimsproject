Code – Hashing and Hash Maps

1. Introduction

For this project, we used C++ to create a Hash Map that stores student IDs and student names.

We split the code into different files to keep everything organised:

main.cpp – Runs the program and tests the Hash Map.

HashMap.h – Contains the HashMap class and its function declarations.

HashMap.cpp – Contains the code for the Hash Map functions.

We also used CMake to build the project.

2. Header File (HashMap.h)

The HashMap.h file contains the HashMap class. It declares the functions that we use in the program.

#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include <list>
#include <utility>

class HashMap
{
private:
    static const int TABLE_SIZE = 10;

    std::list<std::pair<int, std::string>> table[TABLE_SIZE];

    int hashFunction(int key);

public:
    void insert(int key, std::string value);
    std::string search(int key);
    void remove(int key);
    void display();
};

#endif

We set the table size to 10, meaning our Hash Map has 10 buckets.

We used std::list to store the student information in each bucket. Each student is stored as a pair containing an ID and a name.

The functions are declared here and their actual code is written in HashMap.cpp.

3. Hash Function

The hash function takes a student ID and calculates which bucket it should go into.

int HashMap::hashFunction(int key)
{
    return key % TABLE_SIZE;
}

We used the modulo operator % to calculate the bucket number.

For example:

101 % 10 = 1

102 % 10 = 2

111 % 10 = 1

This means student IDs 101 and 111 both go into bucket 1. This creates a collision, which we handle using separate chaining.

4. Insert Function

The insert function adds a new student to the Hash Map.

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

First, the function calculates the correct bucket using the hash function.

It then checks whether the student ID already exists in that bucket.

If the ID already exists, it updates the student's name. Otherwise, it adds the new student to the bucket.

This also means we do not store the same student ID twice.

5. Search Function

The search function looks for a student using their ID.

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

The function first works out which bucket the student should be in.

It then checks the entries in that bucket. If the ID is found, it returns the student's name.

If the ID does not exist, it returns "Student not found".

6. Remove Function

The remove function deletes a student from the Hash Map using their ID.

void HashMap::remove(int key)
{
    int index = hashFunction(key);

    for (auto it = table[index].begin();
         it != table[index].end(); ++it)
    {
        if (it->first == key)
        {
            table[index].erase(it);
            return;
        }
    }
}

The function finds the correct bucket and searches through its entries.

When it finds the matching student ID, it removes that entry from the list.

This is useful when we want to delete a student without affecting other students in the same bucket.

7. Display Function

We also created a display function to show what is stored in the Hash Map.

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

This function goes through all 10 buckets and prints the students stored in each one.

It also helps us see how collisions work because multiple students can appear in the same bucket.

8. Building and Running the Project

We used CMake to build our project.

The CMakeLists.txt file tells CMake which source files to include when building the program.

To build and run the project from the terminal, we can use:

cmake -S . -B build
cmake --build build

Then we can run the executable from the build folder. The exact executable location depends on the build configuration.

9. Testing the Code

We tested the main functions to make sure the Hash Map worked correctly.

Some of the tests included:

Adding students to the Hash Map.

Searching for a student using their ID.

Removing a student.

Adding two students whose IDs produce the same bucket.

Updating a student name when the same ID is inserted again.

Searching for an ID that does not exist.

Removing one student from a bucket containing multiple students.

These tests helped us check that the main operations worked and that collisions were handled correctly.

10. Conclusion

Overall, we used C++ to create a simple Hash Map with separate chaining. We divided the code into different files to make the project easier to understand and maintain.

The program supports inserting, searching, removing and displaying student records. We also tested collisions and duplicate IDs to check that the Hash Map behaved as expected.
