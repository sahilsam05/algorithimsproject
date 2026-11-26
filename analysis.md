# Analysis — Performance of Our Hash Map

[← Back to README](README.md)

**Code used:** [`src/HashMap.cpp`](src/HashMap.cpp) · [`benchmark/benchmark.cpp`](benchmark/benchmark.cpp) · raw results in [`results.csv`](results.csv)

---

## 1. Big-O claims

*n* = number of items stored.

| Operation | Our HashMap (average) | Our HashMap (worst) | `std::map` |
|---|---|---|---|
| insert | O(1) amortised | O(n) | O(log n) |
| search | O(1) | O(n) | O(log n) |
| remove | O(1) | O(n) | O(log n) |
| Space | O(n) | O(n) | O(n) |

- **Average O(1):** the hash function picks a bucket in one step, and resizing keeps about one item per bucket, so each operation only checks a few items.
- **Amortised insert:** resizing copies all *n* items, which is O(n), but it only happens each time the size doubles, so the cost averages out to O(1) per insert.
- **Worst case O(n):** if all keys land in the same bucket, every operation has to scan one long list.

## 2. How we measured

Measurements come from [`benchmark/benchmark.cpp`](benchmark/benchmark.cpp), built in **Release mode**:

- **Data:** *n* distinct random integer keys (fixed seed, so every run is repeatable), at n = 1,000, 10,000, 100,000 and 1,000,000.
- **Operations timed:** insert all keys, search all keys (hits), search keys that aren't there (misses), remove all keys.
- **Timing:** `std::chrono`, reported as **nanoseconds per operation**. Each test runs 3 times and we keep the fastest.
- **Memory:** we count every byte the structure allocates on the heap.
- **Compared against:** `std::map` (red-black tree) and `std::unordered_map` (the standard library's hash map).

**Test machine:** *TODO — CPU, RAM, Windows 11, Visual Studio 2022 (MSVC), x64-Release*

## 3. Results

### 3.1 Time and memory at different sizes

Nanoseconds per operation (lower is better).

| Structure | n | insert | search hit | search miss | remove | bytes / item |
|---|---:|---:|---:|---:|---:|---:|
| HashMap | 1,000 | 558.1 | 13.3 | 13.1 | 57.4 | 148.2 |
| std::unordered_map | 1,000 | 123.0 | 10.6 | 8.9 | 66.1 | 72.5 |
| std::map | 1,000 | 240.0 | 109.1 | 103.0 | 277.7 | 72.1 |
| HashMap | 10,000 | 588.6 | 29.0 | 31.7 | 83.0 | 129.7 |
| std::unordered_map | 10,000 | 367.0 | 46.4 | 41.7 | 147.0 | 82.2 |
| std::map | 10,000 | 1,134.6 | 433.5 | 519.8 | 885.6 | 72.0 |
| HashMap | 100,000 | 1,922.3 | 98.1 | 84.1 | 354.4 | 174.0 |
| std::unordered_map | 100,000 | 393.6 | 62.5 | 55.4 | 238.5 | 77.0 |
| std::map | 100,000 | 815.3 | 860.3 | 814.2 | 1,176.1 | 72.0 |
| HashMap | 1,000,000 | 1,332.6 | 85.6 | 85.0 | 488.1 | 150.4 |
| std::unordered_map | 1,000,000 | 473.4 | 77.9 | 67.0 | 343.6 | 72.8 |
| std::map | 1,000,000 | 1,498.1 | 1,539.9 | 1,524.5 | 2,021.3 | 72.0 |

**What this shows:**
- **Search: our HashMap behaves like O(1).** Search time levels off at about 85–98 ns from 100,000 items upwards, whereas `std::map` keeps rising, from 109 ns to 1,540 ns. At 1,000,000 items our HashMap searches about **18× faster than `std::map`**, and is close to `std::unordered_map` (85.6 vs 77.9 ns).
- **Remove:** at 1,000,000 items our HashMap removes about **4× faster than `std::map`** (488 vs 2,021 ns).
- **The rise in HashMap search time at small sizes comes from the hardware, not the algorithm.** A small table fits in the CPU cache; a large one doesn't, so each lookup has to reach main memory.
- **Insert is our weakness.** Our inserts are about 2.8× slower than `std::unordered_map`'s at 1,000,000 items, and slower than `std::map`'s at 100,000. Each time our table doubles, `rehash()` builds a brand-new table and moves every item into it.
- **Memory: our HashMap uses about twice as much per item** (130–174 bytes vs about 72). Each bucket is a whole `std::list`. In Visual Studio's library even an empty list allocates memory, so every empty bucket costs extra (see [3.2](#32-effect-of-resizing-load-factor)). That's the trade-off: we gain search speed but pay in memory.

### 3.2 Effect of resizing (load factor)

100,000 items in a fixed number of buckets, with resizing turned off.

| Buckets | Items per bucket (load factor) | search hit (ns) | bytes / item |
|---:|---:|---:|---:|
| 1,000 | 100 | 2,699.8 | 56.7 |
| 10,000 | 10 | 202.0 | 63.2 |
| 100,000 | 1 | 58.9 | 128.0 |
| 200,000 | 0.5 | 57.2 | 200.0 |

**What this shows:**
- **More items per bucket means slower searches.** At 100 items per bucket, searches were about **46× slower** than at 1 per bucket. This is why our HashMap doubles its size whenever there's more than 1 item per bucket.
- **Going below 1 per bucket isn't worth it.** Search barely improved (58.9 → 57.2 ns), but memory went up from 128 to 200 bytes per item. A load factor of 1 is the best balance of speed and memory.

### 3.3 Worst case: keys that all collide

20,000 items, 1,000 fixed buckets.

| Keys | Items in fullest bucket | search hit (ns) |
|---|---:|---:|
| Random | small (spread across buckets) | 285.0 |
| Multiples of 1,000 | **all 20,000** | 96,804.4 |

**What this shows:** our hash function is `key % number of buckets`. Every multiple of 1,000 gives a remainder of 0, so **all 20,000 items go into bucket 0** and the hash map turns into one long linked list. Searches became about **340× slower**, which is the O(n) worst case actually happening.

## 4. Inputs that help and inputs that hurt

| Helps ✅ | Hurts ❌ |
|---|---|
| Keys that spread evenly (random or consecutive IDs) | Keys that follow a pattern matching the table size (e.g. multiples of 1,000) |
| About 1 item per bucket, kept by resizing | No resizing, so buckets get long |
| Searching for keys that exist (search stops early) | Searching for missing keys (must scan the whole bucket) |
| Knowing the size in advance (fewer resizes during insert) | Lots of inserts that trigger repeated resizing |

## 5. Conclusion

- **Search and remove** gave constant-time average performance. At 1 million items they were 18× and 4× faster than `std::map`, which supports O(1) vs O(log n).
- With keys that all collide, performance fell to **O(n)**, 340× slower, matching the worst case.
- The costs are **slower inserts** (because of resizing), **about twice the memory** of `std::map`, and **no sorted order**.

These results come from one machine, so exact times will differ elsewhere, but the patterns should be the same.
