// Performance benchmark for HashMap vs std::unordered_map vs std::map.
//
// Measures (per operation, in nanoseconds):
//   insert, search (key present), search (key missing), remove
// and the heap memory each structure uses.
//
// Build in RELEASE mode before trusting the numbers - Debug builds
// are many times slower and the comparison will be misleading.
//
// Results are printed and also saved to results.csv for analysis.md.

#include "HashMap.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <new>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

// Memory measurement: replace global new/delete so every heap allocation
// is counted. Each block stores its size in a small header in front of it.
static std::size_t g_bytesInUse = 0;
static constexpr std::size_t HEADER = alignof(std::max_align_t);

void* operator new(std::size_t size)
{
    void* raw = std::malloc(size + HEADER);
    if (!raw) throw std::bad_alloc();
    *static_cast<std::size_t*>(raw) = size;
    g_bytesInUse += size;
    return static_cast<char*>(raw) + HEADER;
}

void operator delete(void* ptr) noexcept
{
    if (!ptr) return;
    char* raw = static_cast<char*>(ptr) - HEADER;
    g_bytesInUse -= *reinterpret_cast<std::size_t*>(raw);
    std::free(raw);
}

void* operator new[](std::size_t size) { return operator new(size); }
void operator delete[](void* ptr) noexcept { operator delete(ptr); }
void operator delete(void* ptr, std::size_t) noexcept { operator delete(ptr); }
void operator delete[](void* ptr, std::size_t) noexcept { operator delete(ptr); }


// Small adapters so one benchmark function works for all three maps.
static void put(HashMap& m, int k, const std::string& v) { m.insert(k, v); }
static bool has(const HashMap& m, int k) { return m.contains(k); }
static void del(HashMap& m, int k) { m.remove(k); }

template <class M> void put(M& m, int k, const std::string& v) { m[k] = v; }
template <class M> bool has(const M& m, int k) { return m.find(k) != m.end(); }
template <class M> void del(M& m, int k) { m.erase(k); }

using Clock = std::chrono::steady_clock;

struct Result
{
    std::string experiment;
    std::string structure;
    std::size_t n = 0;
    double insertNs = 0, hitNs = 0, missNs = 0, removeNs = 0;
    std::size_t memoryBytes = 0;
};

static volatile std::size_t g_sink = 0;   // stops the compiler deleting "unused" work

static double nsPerOp(Clock::time_point start, Clock::time_point end, std::size_t ops)
{
    return std::chrono::duration<double, std::nano>(end - start).count() / ops;
}

// Runs the full insert/search/remove cycle `repeats` times and keeps the
// fastest time for each operation (the least affected by background noise).
template <class Map, class MakeMap>
Result runBenchmark(const std::string& experiment, const std::string& name, MakeMap makeMap,
    const std::vector<int>& keys, const std::vector<int>& missing,
    const std::vector<std::string>& values, int repeats)
{
    Result best;
    best.experiment = experiment;
    best.structure = name;
    best.n = keys.size();
    best.insertNs = best.hitNs = best.missNs = best.removeNs = 1e300;

    // Search and remove in a different order from insertion
    std::vector<int> lookupOrder = keys;
    std::shuffle(lookupOrder.begin(), lookupOrder.end(), std::mt19937(7));

    for (int r = 0; r < repeats; r++)
    {
        std::size_t memBefore = g_bytesInUse;
        Map map = makeMap();

        auto t0 = Clock::now();
        for (std::size_t i = 0; i < keys.size(); i++) put(map, keys[i], values[i]);
        auto t1 = Clock::now();
        best.memoryBytes = g_bytesInUse - memBefore;

        std::size_t found = 0;
        auto t2 = Clock::now();
        for (int k : lookupOrder) found += has(map, k);
        auto t3 = Clock::now();
        for (int k : missing) found += has(map, k);
        auto t4 = Clock::now();
        for (int k : lookupOrder) del(map, k);
        auto t5 = Clock::now();
        g_sink = g_sink + found;

        if (found != keys.size())
            std::cerr << "WARNING: " << name << " found " << found << " of " << keys.size() << "\n";

        best.insertNs = std::min(best.insertNs, nsPerOp(t0, t1, keys.size()));
        best.hitNs = std::min(best.hitNs, nsPerOp(t2, t3, lookupOrder.size()));
        best.missNs = std::min(best.missNs, nsPerOp(t3, t4, missing.size()));
        best.removeNs = std::min(best.removeNs, nsPerOp(t4, t5, lookupOrder.size()));
    }
    return best;
}

// Generates n distinct random keys, plus n more keys that are NOT in the set.
static void makeKeys(std::size_t n, std::vector<int>& keys, std::vector<int>& missing)
{
    std::mt19937 rng(42);   // fixed seed -> same keys every run (repeatable)
    std::uniform_int_distribution<int> dist(0, 2000000000);

    std::vector<int> all;
    while (all.size() < 2 * n)
    {
        while (all.size() < 2 * n) all.push_back(dist(rng));
        std::sort(all.begin(), all.end());
        all.erase(std::unique(all.begin(), all.end()), all.end());
    }
    std::shuffle(all.begin(), all.end(), rng);

    keys.assign(all.begin(), all.begin() + n);
    missing.assign(all.begin() + n, all.begin() + 2 * n);
}

static std::vector<std::string> makeValues(const std::vector<int>& keys)
{
    std::vector<std::string> values;
    values.reserve(keys.size());
    for (std::size_t i = 0; i < keys.size(); i++) values.push_back("Student " + std::to_string(i));
    return values;
}

static void printHeader()
{
    std::cout << std::left << std::setw(30) << "Structure" << std::right
        << std::setw(10) << "N"
        << std::setw(12) << "insert"
        << std::setw(12) << "search hit"
        << std::setw(13) << "search miss"
        << std::setw(12) << "remove"
        << std::setw(14) << "bytes/item" << "\n"
        << std::string(103, '-') << "\n";
}

static void printRow(const Result& r)
{
    std::cout << std::left << std::setw(30) << r.structure << std::right
        << std::setw(10) << r.n << std::fixed << std::setprecision(1)
        << std::setw(12) << r.insertNs
        << std::setw(12) << r.hitNs
        << std::setw(13) << r.missNs
        << std::setw(12) << r.removeNs
        << std::setw(14) << static_cast<double>(r.memoryBytes) / r.n << "\n";
}

int main()
{
    std::vector<Result> results;
    const int REPEATS = 3;

    std::cout << "All times are nanoseconds per operation (lower is better).\n";

    
    // Experiment 1: our HashMap vs std::unordered_map vs std::map
    std::cout << "\n=== Experiment 1: comparison at different input sizes ===\n";
    printHeader();

    for (std::size_t n : { 1000u, 10000u, 100000u, 1000000u })
    {
        std::vector<int> keys, missing;
        makeKeys(n, keys, missing);
        auto values = makeValues(keys);

        results.push_back(runBenchmark<HashMap>("size", "HashMap (resizing)",
            [] { return HashMap(); }, keys, missing, values, REPEATS));
        printRow(results.back());

        // A fixed table of 10 buckets gets O(n) chains - too slow for large n
        if (n <= 10000)
        {
            results.push_back(runBenchmark<HashMap>("size", "HashMap (fixed 10 buckets)",
                [] { return HashMap(10, false); }, keys, missing, values, REPEATS));
            printRow(results.back());
        }

        results.push_back(runBenchmark<std::unordered_map<int, std::string>>("size",
            "std::unordered_map", [] { return std::unordered_map<int, std::string>(); },
            keys, missing, values, REPEATS));
        printRow(results.back());

        results.push_back(runBenchmark<std::map<int, std::string>>("size", "std::map",
            [] { return std::map<int, std::string>(); }, keys, missing, values, REPEATS));
        printRow(results.back());
        std::cout << "\n";
    }

    
    // Experiment 2: effect of load factor (items per bucket)
    std::cout << "=== Experiment 2: load factor (N = 100000, fixed bucket count) ===\n";
    {
        const std::size_t n = 100000;
        std::vector<int> keys, missing;
        makeKeys(n, keys, missing);
        auto values = makeValues(keys);

        std::cout << std::left << std::setw(12) << "Buckets" << std::right
            << std::setw(13) << "load factor" << std::setw(16) << "longest chain"
            << std::setw(12) << "insert" << std::setw(12) << "search hit"
            << std::setw(13) << "search miss" << "\n" << std::string(78, '-') << "\n";

        for (std::size_t buckets : { 1000u, 10000u, 100000u, 200000u })
        {
            Result r = runBenchmark<HashMap>("load_factor",
                "HashMap " + std::to_string(buckets) + " buckets",
                [buckets] { return HashMap(buckets, false); }, keys, missing, values, REPEATS);
            results.push_back(r);

            HashMap stats(buckets, false);
            for (std::size_t i = 0; i < n; i++) stats.insert(keys[i], values[i]);

            std::cout << std::left << std::setw(12) << buckets << std::right << std::fixed
                << std::setprecision(2) << std::setw(13) << stats.loadFactor()
                << std::setw(16) << stats.longestBucket() << std::setprecision(1)
                << std::setw(12) << r.insertNs << std::setw(12) << r.hitNs
                << std::setw(13) << r.missNs << "\n";
        }
    }


    // Experiment 3: inputs that hurt - keys that all collide
    std::cout << "\n=== Experiment 3: good vs bad keys (N = 20000, 1000 fixed buckets) ===\n";
    {
        const std::size_t n = 20000;
        std::vector<int> randomKeys, missing;
        makeKeys(n, randomKeys, missing);
        auto values = makeValues(randomKeys);

        // Every key is a multiple of 1000, so key % 1000 == 0 -> all in bucket 0
        std::vector<int> badKeys(n), badMissing(n);
        for (std::size_t i = 0; i < n; i++)
        {
            badKeys[i] = static_cast<int>(i * 1000);
            badMissing[i] = static_cast<int>((n + i) * 1000);
        }
        std::shuffle(badKeys.begin(), badKeys.end(), std::mt19937(1));

        std::cout << std::left << std::setw(28) << "Keys" << std::right
            << std::setw(16) << "longest chain" << std::setw(12) << "insert"
            << std::setw(12) << "search hit" << std::setw(13) << "search miss"
            << "\n" << std::string(81, '-') << "\n";

        struct Case { std::string label; const std::vector<int>* keys; const std::vector<int>* miss; };
        for (const Case& c : { Case{ "random (spread out)", &randomKeys, &missing },
                               Case{ "multiples of 1000 (collide)", &badKeys, &badMissing } })
        {
            Result r = runBenchmark<HashMap>("bad_keys", "HashMap - " + c.label,
                [] { return HashMap(1000, false); }, *c.keys, *c.miss, values, 1);
            results.push_back(r);

            HashMap stats(1000, false);
            for (std::size_t i = 0; i < n; i++) stats.insert((*c.keys)[i], values[i]);

            std::cout << std::left << std::setw(28) << c.label << std::right
                << std::setw(16) << stats.longestBucket() << std::fixed
                << std::setprecision(1) << std::setw(12) << r.insertNs
                << std::setw(12) << r.hitNs << std::setw(13) << r.missNs << "\n";
        }
    }


    // Save everything to CSV
    std::ofstream csv("results.csv");
    csv << "experiment,structure,n,insert_ns,search_hit_ns,search_miss_ns,remove_ns,memory_bytes,bytes_per_item\n";
    for (const Result& r : results)
    {
        csv << r.experiment << "," << r.structure << "," << r.n << "," << r.insertNs << ","
            << r.hitNs << "," << r.missNs << "," << r.removeNs << "," << r.memoryBytes << ","
            << static_cast<double>(r.memoryBytes) / r.n << "\n";
    }
    std::cout << "\nResults saved to results.csv\n";
    return 0;
}
