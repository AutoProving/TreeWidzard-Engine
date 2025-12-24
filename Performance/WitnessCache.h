#ifndef WITNESS_CACHE_H
#define WITNESS_CACHE_H

#include "../Kernel/Bag.h"
#include "../Kernel/State.h"
#include "../Kernel/WitnessSet.h"
#include "../parallel_hashmap/phmap.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <unordered_map>

namespace TreeWidzard {

// Hash function for Bag objects
struct BagHash {
  std::size_t operator()(const Bag &bag) const {
    std::size_t hash = 0;
    auto vertices = bag.get_elements();

    // Use a commutative hash function since bag order doesn't matter
    for (int vertex : vertices) {
      hash ^= std::hash<int>{}(vertex) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
  }
};

// Cache entry with metadata
template <typename ValueType> struct CacheEntry {
  ValueType value;
  std::chrono::steady_clock::time_point last_access;
  std::atomic<uint64_t> access_count;
  size_t computation_cost; // Estimated cost to recompute

  CacheEntry(ValueType &&val, size_t cost = 1)
      : value(std::move(val)), last_access(std::chrono::steady_clock::now()),
        access_count(1), computation_cost(cost) {}
};

// Thread-safe LRU cache with cost-aware eviction
template <typename KeyType, typename ValueType,
          typename HashType = std::hash<KeyType>>
class PerformanceCache {
private:
  using EntryType = CacheEntry<ValueType>;
  using MapType = phmap::parallel_flat_hash_map<
      KeyType, std::unique_ptr<EntryType>, HashType, std::equal_to<KeyType>,
      std::allocator<std::pair<const KeyType, std::unique_ptr<EntryType>>>,
      4, // 4 submaps for good parallel performance
      std::mutex>;

  MapType cache_map;
  std::atomic<size_t> max_size;
  std::atomic<size_t> current_size;
  std::atomic<uint64_t> hits;
  std::atomic<uint64_t> misses;
  std::atomic<uint64_t> evictions;

  // Background cleanup thread
  std::thread cleanup_thread;
  std::atomic<bool> cleanup_running;
  std::chrono::seconds cleanup_interval;

public:
  explicit PerformanceCache(
      size_t max_entries = 10000,
      std::chrono::seconds cleanup_freq = std::chrono::seconds(60))
      : max_size(max_entries), current_size(0), hits(0), misses(0),
        evictions(0), cleanup_running(true), cleanup_interval(cleanup_freq) {

    // Start background cleanup thread
    cleanup_thread = std::thread([this]() {
      while (cleanup_running) {
        std::this_thread::sleep_for(cleanup_interval);
        if (cleanup_running) {
          performCleanup();
        }
      }
    });
  }

  ~PerformanceCache() {
    cleanup_running = false;
    if (cleanup_thread.joinable()) {
      cleanup_thread.join();
    }
  }

  // Insert or update cache entry
  void insert(const KeyType &key, ValueType &&value,
              size_t computation_cost = 1) {
    auto entry =
        std::make_unique<EntryType>(std::move(value), computation_cost);

    bool inserted = false;
    cache_map.lazy_emplace_l(
        key,
        [&](MapType::value_type &p) {
          // Update existing entry
          p.second = std::move(entry);
          p.second->last_access = std::chrono::steady_clock::now();
          p.second->access_count++;
        },
        [&](const MapType::constructor &ctor) {
          // Insert new entry
          ctor(key, std::move(entry));
          current_size++;
          inserted = true;
        });

    // Trigger cleanup if cache is too large
    if (inserted && current_size > max_size) {
      performCleanup();
    }
  }

  // Retrieve from cache
  std::optional<ValueType> get(const KeyType &key) {
    std::optional<ValueType> result;

    cache_map.if_contains(key, [&](const MapType::value_type &p) {
      result = p.second->value;
      p.second->last_access = std::chrono::steady_clock::now();
      p.second->access_count++;
      hits++;
    });

    if (!result) {
      misses++;
    }

    return result;
  }

  // Check if key exists
  bool contains(const KeyType &key) const { return cache_map.contains(key); }

  // Cache statistics
  struct Statistics {
    uint64_t hits;
    uint64_t misses;
    uint64_t evictions;
    size_t current_size;
    size_t max_size;
    double hit_ratio;
    double average_cost;
  };

  Statistics getStatistics() const {
    uint64_t total_hits = hits.load();
    uint64_t total_misses = misses.load();
    uint64_t total_requests = total_hits + total_misses;

    double hit_ratio = total_requests > 0
                           ? static_cast<double>(total_hits) / total_requests
                           : 0.0;

    // Calculate average computation cost
    double avg_cost = 0.0;
    size_t count = 0;
    cache_map.for_each([&](const MapType::value_type &p) {
      avg_cost += p.second->computation_cost;
      count++;
    });
    avg_cost = count > 0 ? avg_cost / count : 0.0;

    return {
        total_hits,      total_misses, evictions.load(), current_size.load(),
        max_size.load(), hit_ratio,    avg_cost};
  }

  // Clear cache
  void clear() {
    cache_map.clear();
    current_size = 0;
    hits = 0;
    misses = 0;
    evictions = 0;
  }

  // Resize cache
  void resize(size_t new_max_size) {
    max_size = new_max_size;
    if (current_size > max_size) {
      performCleanup();
    }
  }

private:
  void performCleanup() {
    if (current_size <= max_size * 0.8) {
      return; // No cleanup needed
    }

    auto now = std::chrono::steady_clock::now();
    std::vector<std::pair<KeyType, double>> candidates;

    // Collect eviction candidates with scores
    cache_map.for_each([&](const MapType::value_type &p) {
      auto age = std::chrono::duration_cast<std::chrono::seconds>(
                     now - p.second->last_access)
                     .count();

      // Score based on recency, frequency, and computation cost
      double recency_score = 1.0 / (1.0 + age);
      double frequency_score = std::log(1.0 + p.second->access_count.load());
      double cost_score = std::log(1.0 + p.second->computation_cost);

      // Lower score = more likely to be evicted
      double score = frequency_score * cost_score * recency_score;
      candidates.emplace_back(p.first, score);
    });

    // Sort by score (ascending - lowest scores first)
    std::sort(candidates.begin(), candidates.end(),
              [](const auto &a, const auto &b) { return a.second < b.second; });

    // Remove lowest-scoring entries until we're under the limit
    size_t target_size = max_size * 0.7; // Remove to 70% capacity
    size_t to_remove =
        current_size > target_size ? current_size - target_size : 0;

    for (size_t i = 0; i < std::min(to_remove, candidates.size()); ++i) {
      if (cache_map.erase(candidates[i].first)) {
        current_size--;
        evictions++;
      }
    }
  }
};

// Specialized witness set cache
class WitnessSetCache {
private:
  PerformanceCache<Bag, std::shared_ptr<WitnessSet>, BagHash> cache;
  std::atomic<bool> enabled;

public:
  explicit WitnessSetCache(size_t max_entries = 50000)
      : cache(max_entries), enabled(true) {}

  using Statistics = typename PerformanceCache<Bag, std::shared_ptr<WitnessSet>,
                                               BagHash>::Statistics;

  void insert(const Bag &bag, std::shared_ptr<WitnessSet> witness_set,
              size_t computation_cost = 1) {
    if (enabled.load()) {
      cache.insert(bag, std::move(witness_set), computation_cost);
    }
  }

  std::optional<std::shared_ptr<WitnessSet>> get(const Bag &bag) {
    if (enabled.load()) {
      return cache.get(bag);
    }
    return std::nullopt;
  }

  bool contains(const Bag &bag) const {
    return enabled.load() && cache.contains(bag);
  }

  auto getStatistics() const { return cache.getStatistics(); }
  void clear() { cache.clear(); }
  void setEnabled(bool enable) { enabled = enable; }
  bool isEnabled() const { return enabled.load(); }
};

// DP computation result cache
struct DPResult {
  double value;
  bool is_satisfiable;
  std::vector<State> witness_states;

  DPResult(double v, bool sat) : value(v), is_satisfiable(sat) {}
  DPResult(double v, bool sat, std::vector<State> &&states)
      : value(v), is_satisfiable(sat), witness_states(std::move(states)) {}
};

// Hash for DP computation keys
struct DPComputationKey {
  Bag bag;
  std::string core_name;
  std::vector<int> parameters;

  bool operator==(const DPComputationKey &other) const {
    return bag == other.bag && core_name == other.core_name &&
           parameters == other.parameters;
  }
};

struct DPComputationKeyHash {
  std::size_t operator()(const DPComputationKey &key) const {
    std::size_t hash = BagHash{}(key.bag);
    hash ^= std::hash<std::string>{}(key.core_name) + 0x9e3779b9 + (hash << 6) +
            (hash >> 2);

    for (int param : key.parameters) {
      hash ^= std::hash<int>{}(param) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
  }
};

using DPComputationCache =
    PerformanceCache<DPComputationKey, DPResult, DPComputationKeyHash>;

// Centralized cache manager
class CacheManager {
private:
  static std::unique_ptr<CacheManager> instance;
  static std::once_flag init_flag;

  WitnessSetCache witness_cache;
  DPComputationCache dp_cache;
  std::atomic<bool> global_caching_enabled;

  CacheManager() : global_caching_enabled(true) {}

public:
  static CacheManager &getInstance() {
    std::call_once(init_flag, []() {
      instance = std::unique_ptr<CacheManager>(new CacheManager());
    });
    return *instance;
  }

  WitnessSetCache &getWitnessCache() { return witness_cache; }
  DPComputationCache &getDPCache() { return dp_cache; }

  void enableCaching(bool enable = true) {
    global_caching_enabled = enable;
    witness_cache.setEnabled(enable);
  }

  bool isCachingEnabled() const { return global_caching_enabled.load(); }

  struct GlobalStatistics {
    WitnessSetCache::Statistics witness_stats;
    DPComputationCache::Statistics dp_stats;
    size_t total_memory_usage;
  };

  GlobalStatistics getGlobalStatistics() const {
    // Estimate memory usage (simplified)
    auto witness_stats = witness_cache.getStatistics();
    auto dp_stats = dp_cache.getStatistics();

    size_t estimated_memory = witness_stats.current_size * sizeof(WitnessSet) +
                              dp_stats.current_size * sizeof(DPResult);

    return {witness_stats, dp_stats, estimated_memory};
  }

  void clearAllCaches() {
    witness_cache.clear();
    dp_cache.clear();
  }
};

} // namespace TreeWidzard

#endif // WITNESS_CACHE_H