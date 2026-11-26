# Overview — Hashing & Hash Maps

[← Back to README](README.md)

---

## 1. The problem

Many programs keep asking one question: **"Given this key, where is its value?"**

- Given a username, find the account.
- Given a word, find how many times it has appeared.
- Given a URL, check whether we already have the page cached.

The obvious data structures handle this badly when the data gets large:

| Approach | Find a key | Problem |
|---|---|---|
| Unsorted array / `std::vector` | O(n) | Every lookup scans the whole list |
| Sorted array + binary search | O(log n) | Inserting and removing are O(n) because elements must shift |
| Balanced BST (`std::map`) | O(log n) | Good, but every lookup still walks a path of comparisons |

What we want is to go **straight to the right location** without searching. Hashing makes that possible.

## 2. What a hash map is

A **hash map** (also called a hash table, dictionary or associative array) stores **key → value** pairs in an array of *buckets*.

1. A **hash function** turns the key into an integer, e.g. `hash("apple") → 93847562`.
2. That integer is reduced to an array index: `index = hash % capacity`.
3. The pair is stored in (or looked up from) that bucket.

```
key "apple" ──hash()──► 93847562 ──% 8──► bucket 2

buckets: [0] [1] [2:"apple"→5] [3] [4] [5] [6] [7]
```

Because computing the index takes the same time no matter how many items are stored, **insert, search and remove are O(1) on average**.

The catch is **collisions**: two different keys can land in the same bucket. Every hash map needs a collision strategy (e.g. *separate chaining* or *open addressing*). It also needs to **resize** as it fills up, so buckets don't get overcrowded. These ideas are explained in [design.md](design.md).

## 3. Real-world use-cases

### 3.1 Caching (e.g. web caches, memoisation)
A cache maps a request (a URL, or a function's arguments) to a previously computed result. Before doing expensive work, the program checks the hash map. A hit returns the answer in O(1); a miss computes it and stores it. Browsers, content delivery networks and in-memory stores like Redis rely on this idea. In dynamic programming, memoisation is often just a hash map from sub-problem to answer.

### 3.2 Counting and frequency analysis
Counting word frequencies in a document, page visits per user, or votes per candidate all follow the same pattern:

```cpp
std::unordered_map<std::string, int> counts;
for (const auto& word : words) {
    ++counts[word];   // O(1) average per word
}
```

With a hash map, counting n items takes O(n) overall. A sorted structure would take O(n log n), and repeatedly scanning a list would take O(n²).

### 3.3 Symbol tables and indexing
Compilers and interpreters use hash maps to map variable and function names to their type, scope and memory location. Databases use **hash indexes** to find rows by an exact key, and programming languages use hash maps to implement objects and dictionaries (Python's `dict`, JavaScript objects, C++'s `std::unordered_map`).

### 3.4 De-duplication and membership tests
A **hash set** (a hash map with keys only) answers "have I seen this before?" in O(1) average time. It is used for removing duplicate records, detecting repeated transactions, and tracking visited nodes in graph searches.

## 4. When to use a hash map (and when not to)

**Use a hash map when:**
- You mostly look items up by an **exact key**.
- You need **fast average-case** insert, search and remove.
- The **order of keys doesn't matter**.
- You have a **good hash function** for your key type (built-in for integers and strings in C++).

**Consider something else when:**

| Need | Better choice | Why |
|---|---|---|
| Keys in sorted order, or range queries ("all keys between A and F") | `std::map` (red-black tree) | Hash maps scatter keys and have no ordering |
| A **guaranteed** worst-case time (e.g. real-time systems) | Balanced BST, O(log n) worst case | A hash map's worst case is O(n) when many keys collide |
| Very small data sets | `std::vector` + linear scan | Hashing overhead can be slower than scanning a few elements, and vectors are cache-friendly |
| Memory is very tight | Sorted array | Hash maps keep empty buckets to keep the load factor low |
| Keys are small dense integers (0…n) | Plain array or bit vector | The key *is* the index, so no hashing is needed |

We compare our hash map against `std::map` in [analysis.md](analysis.md).

## 5. Why we chose this topic

- **It's everywhere.** Hash maps are among the most used data structures in real software. Most programmers use them every day through `std::unordered_map`, `dict` or `HashMap`, but few know what happens inside.
- **It teaches several core ideas at once:** hash functions, collision handling, load factor, amortised analysis (resizing) and the gap between average and worst case.
- **The performance is easy to measure.** We can show O(1) average behaviour, then make it collapse to O(n) with a deliberately bad hash function. This gives meaningful benchmark results.
- **It has a clear comparison point.** Comparing it with the tree-based `std::map` shows a real trade-off: speed against ordering and guarantees.

## 6. Scope of this project

In this project we:

- Implement our own hash map in C++ using **separate chaining**, with `insert`, `search` and `remove`, plus **automatic resizing** based on load factor.
- Explain hash functions and collision strategies, with diagrams ([design.md](design.md)).
- Test the implementation and give build/run instructions ([code.md](code.md)).
- Measure performance against `std::unordered_map` and `std::map` and compare good and bad hash functions ([analysis.md](analysis.md)).

Out of scope: cryptographic hash functions (SHA-256 etc.), concurrent/thread-safe hash maps, and distributed hashing.
