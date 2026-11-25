// Unit tests for HashMap.
// Uses a small CHECK macro instead of assert(), because assert() is
// switched off in Release builds.

#include "HashMap.h"

#include <iostream>
#include <string>

static int passed = 0;
static int failed = 0;

#define CHECK(condition)                                                   \
    do {                                                                   \
        if (condition) { ++passed; }                                       \
        else {                                                             \
            ++failed;                                                      \
            std::cout << "  FAILED: " << #condition                        \
                      << " (line " << __LINE__ << ")\n";                   \
        }                                                                  \
    } while (0)

static void testInsertManyStudents()
{
    std::cout << "Test: insert many students\n";
    HashMap map;
    const int N = 10000;

    for (int id = 0; id < N; id++)
        map.insert(id, "Student " + std::to_string(id));

    CHECK(map.size() == static_cast<std::size_t>(N));

    bool allFound = true;
    for (int id = 0; id < N; id++)
        if (map.search(id) != "Student " + std::to_string(id)) allFound = false;
    CHECK(allFound);

    // Resizing should have kept the load factor at or below 1.0
    CHECK(map.loadFactor() <= 1.0);
    CHECK(map.bucketCount() >= static_cast<std::size_t>(N));
}

static void testSearchExistingAndMissing()
{
    std::cout << "Test: search existing and missing IDs\n";
    HashMap map;
    map.insert(101, "Sahil");
    map.insert(102, "Daniel");

    CHECK(map.search(101) == "Sahil");
    CHECK(map.search(102) == "Daniel");
    CHECK(map.search(999) == "Student not found");
    CHECK(map.contains(101));
    CHECK(!map.contains(999));

    HashMap empty;
    CHECK(empty.search(1) == "Student not found");
    CHECK(empty.size() == 0);
}

static void testUpdateDuplicateKeys()
{
    std::cout << "Test: updating duplicate keys\n";
    HashMap map;
    map.insert(101, "Sahil");
    map.insert(101, "Sahil Updated");

    CHECK(map.search(101) == "Sahil Updated");
    CHECK(map.size() == 1);   // updated, not added twice
}

static void testCollisionScenarios()
{
    std::cout << "Test: collision scenarios\n";

    // Fixed table of 10 buckets: 1, 11, 21, 31, 41 all hash to bucket 1
    HashMap map(10, false);
    for (int id = 1; id <= 41; id += 10)
        map.insert(id, "S" + std::to_string(id));

    CHECK(map.size() == 5);
    CHECK(map.longestBucket() == 5);   // all five in one chain
    CHECK(map.search(1) == "S1");
    CHECK(map.search(21) == "S21");
    CHECK(map.search(41) == "S41");
    CHECK(!map.contains(51));          // same bucket, but not present

    // Update a key that is in the middle of a chain
    map.insert(21, "Updated");
    CHECK(map.search(21) == "Updated");
    CHECK(map.size() == 5);

    // Worst case: every key collides
    HashMap worst(10, false);
    for (int i = 0; i < 1000; i++)
        worst.insert(i * 10, "x");     // all multiples of 10 -> bucket 0
    CHECK(worst.longestBucket() == 1000);
    CHECK(worst.contains(9990));
    CHECK(!worst.contains(5));
}

static void testDeleteFromCollisionBuckets()
{
    std::cout << "Test: deleting from collision buckets\n";
    HashMap map(10, false);
    map.insert(3, "A");    // all in bucket 3
    map.insert(13, "B");
    map.insert(23, "C");
    map.insert(33, "D");

    CHECK(map.remove(23));             // middle of chain
    CHECK(!map.contains(23));
    CHECK(map.search(13) == "B");
    CHECK(map.search(33) == "D");

    CHECK(map.remove(3));              // head of chain
    CHECK(map.remove(33));             // tail of chain
    CHECK(map.search(13) == "B");
    CHECK(map.size() == 1);

    CHECK(!map.remove(43));            // not present
    CHECK(!map.remove(23));            // already removed
    CHECK(map.size() == 1);

    // Re-insert after removal
    map.insert(23, "C again");
    CHECK(map.search(23) == "C again");
}

static void testNegativeKeys()
{
    std::cout << "Test: negative keys\n";
    HashMap map;
    map.insert(-7, "Negative");
    map.insert(-17, "Also negative");
    CHECK(map.search(-7) == "Negative");
    CHECK(map.search(-17) == "Also negative");
    CHECK(map.remove(-7));
    CHECK(!map.contains(-7));
}

static void testResizeKeepsData()
{
    std::cout << "Test: resizing keeps all data\n";
    HashMap map(2);   // starts tiny so it has to resize many times
    for (int id = 0; id < 500; id++)
        map.insert(id * 7, std::to_string(id));

    CHECK(map.size() == 500);
    CHECK(map.bucketCount() >= 500);
    CHECK(map.search(0) == "0");
    CHECK(map.search(499 * 7) == "499");

    // Removing everything leaves an empty map
    for (int id = 0; id < 500; id++)
        map.remove(id * 7);
    CHECK(map.size() == 0);
    CHECK(!map.contains(7));
}

int main()
{
    testInsertManyStudents();
    testSearchExistingAndMissing();
    testUpdateDuplicateKeys();
    testCollisionScenarios();
    testDeleteFromCollisionBuckets();
    testNegativeKeys();
    testResizeKeepsData();

    std::cout << "\n" << passed << " checks passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
