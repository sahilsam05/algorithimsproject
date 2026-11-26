# Code – Hashing and Hash Maps

## 1. Introduction

For this project, we used C++ to create a Hash Map that stores student IDs and student names.

We split the code into different files to keep everything organised and easier to understand.

Our project contains:

- `HashMap.h` – Contains the HashMap class and function declarations.
- `HashMap.cpp` – Contains the actual code for the Hash Map functions.
- `main.cpp` – Runs the program and demonstrates how the Hash Map works.
- `tests.cpp` – Contains tests for checking the different functions.
- `benchmark.cpp` – Used to measure and compare performance.

We also used CMake to build the project.

## 2. Header File (HashMap.h)

The `HashMap.h` file contains our HashMap class.

It declares the functions and variables that are used throughout the project.

Our main storage structure is:

```cpp
std::vector<std::list<std::pair<int, std::string>>> table;
```

We use a vector to store the buckets and a linked list inside each bucket.

Each entry is a pair containing a student ID and a name.

We also use a counter to keep track of how many records are stored:

```cpp
std::size_t count = 0;
```

The class includes functions for inserting, searching, removing and displaying student records.

It also includes additional functions for checking the size, bucket count, load factor and longest bucket.

## 3. Hash Function

The hash function takes a student ID and calculates which bucket it should go into.

This is the code from our `HashMap.cpp` file:

```cpp
std::size_t HashMap::hashFunction(int key) const
{
    return static_cast<unsigned int>(key) % table.size();
}
```

We use the modulo operator `%` to calculate the bucket number.

For example, if we have 10 buckets:

- 101 % 10 = 1
- 102 % 10 = 2
- 103 % 10 = 3
- 111 % 10 = 1

This means student IDs 101 and 111 both go into bucket 1.

This creates a collision, which our program handles using separate chaining.

## 4. Insert Function

The insert function adds a new student to the Hash Map.

```cpp
void HashMap::insert(int key, const std::string& value)
{
    std::size_t index = hashFunction(key);

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
```

First, the function calculates the correct bucket using the hash function.

It then checks whether the student ID already exists.

If the ID already exists, it updates the student's name. Otherwise, it adds a new record and increases the record count.

Finally, it checks the load factor. If the load factor is greater than 1.0 and automatic resizing is enabled, the Hash Map doubles its bucket count.

## 5. Search Function

The search function looks for a student using their ID.

```cpp
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
```

The function calculates which bucket the student should be in.

It then searches through the entries in that bucket.

If the student ID is found, it returns their name. If the ID does not exist, it returns `"Student not found"`.

## 6. Remove Function

The remove function deletes a student using their ID.

```cpp
bool HashMap::remove(int key)
{
    std::size_t index = hashFunction(key);

    for (auto it = table[index].begin();
         it != table[index].end(); ++it)
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
```

The function finds the correct bucket and searches for the matching student ID.

If the student is found, it removes the record and decreases the total count.

It returns `true` if the student was removed and `false` if the ID was not found.

## 7. Automatic Resizing and Rehashing

One of the improvements we made was adding automatic resizing.

We use the load factor to measure how full the Hash Map is.

```cpp
double HashMap::loadFactor() const
{
    return static_cast<double>(count) /
           static_cast<double>(table.size());
}
```

Our maximum load factor is set to 1.0.

When the load factor goes above 1.0, the Hash Map doubles its bucket count.

We use the `rehash()` function to move the existing records into their new bucket positions.

This is important because changing the number of buckets also changes the results of the hash function.

By resizing the Hash Map, we can reduce the average number of records stored in each bucket as more students are added.

## 8. Other Functions

We also added some extra functions to help manage and understand the Hash Map.

- `contains()` checks whether a student ID exists and returns true or false.
- `size()` returns the total number of student records.
- `bucketCount()` returns the current number of buckets.
- `loadFactor()` calculates how full the Hash Map is.
- `longestBucket()` returns the number of entries in the bucket containing the most records.
- `display()` prints all the buckets and the student records stored inside them.

These functions help us check how the Hash Map is performing and how the records are distributed.

## 9. Main Program

The `main.cpp` file demonstrates how our Hash Map works.

We create a Hash Map and insert several student records.

We then demonstrate searching for students, updating an existing student record and removing students.

We also display the Hash Map so we can see how the records are organised into buckets.

For example, we insert student IDs 101 and 111 to demonstrate how collisions are handled.

## 10. Testing

We created a separate `tests.cpp` file to test our Hash Map.

The tests check different situations, including:

- Inserting student records.
- Searching for existing and missing IDs.
- Removing records.
- Handling collisions.
- Updating duplicate IDs.
- Checking the size of the Hash Map.
- Checking the load factor.
- Testing automatic resizing.
- Checking the longest bucket.

This helped us make sure the different functions were working correctly.

## 11. Performance Benchmark

We also created a separate `benchmark.cpp` file.

The purpose of this file is to measure how our Hash Map performs and compare it with other C++ data structures.

We compare our implementation against:

- `std::unordered_map`
- `std::map`

We use different amounts of data and different input patterns to see how performance changes.

The actual benchmark results and performance analysis are explained in `analysis.md`.

## 12. Building and Running the Project

We used CMake to organise and build the project.

The `CMakeLists.txt` file tells CMake which source files to compile and which executables to create.

To configure the project, we use:

```bash
cmake -S . -B build
```

To build everything:

```bash
cmake --build build
```

To run the tests:

```bash
ctest --test-dir build --output-on-failure
```

The project creates three executables:

- `demo` – Runs the main Hash Map demonstration.
- `tests` – Runs the unit tests.
- `benchmark` – Runs the performance benchmarks.

The executable location depends on the build configuration and compiler.

## 13. Conclusion

Overall, we used C++ to create our own Hash Map using separate chaining.

We implemented the main operations and added extra features such as automatic resizing, rehashing and load factor calculations.

We also created separate tests and benchmarks to check how the Hash Map works and compare its performance with other data structures.
