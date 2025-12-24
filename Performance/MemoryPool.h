#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

// Forward declarations
class State;
class WitnessSet;
class Bag;
class ConjectureNode;

namespace TreeWidzard {

// Thread-safe memory pool for objects of type T
template <typename T> class MemoryPool {
private:
  struct Block {
    alignas(T) char data[sizeof(T)];
    Block *next;

    Block() : next(nullptr) {}
  };

  std::vector<std::unique_ptr<Block[]>> chunks;
  std::queue<Block *> available_blocks;
  mutable std::mutex pool_mutex;

  size_t chunk_size;
  size_t total_allocated;
  size_t total_in_use;

  static constexpr size_t DEFAULT_CHUNK_SIZE = 1024;

public:
  explicit MemoryPool(size_t initial_chunk_size = DEFAULT_CHUNK_SIZE)
      : chunk_size(initial_chunk_size), total_allocated(0), total_in_use(0) {
    allocateNewChunk();
  }

  ~MemoryPool() {
    // Destructor will automatically clean up all chunks
  }

  // Acquire an object from the pool
  template <typename... Args> T *acquire(Args &&...args) {
    std::lock_guard<std::mutex> lock(pool_mutex);

    if (available_blocks.empty()) {
      allocateNewChunk();
    }

    Block *block = available_blocks.front();
    available_blocks.pop();
    total_in_use++;

    // Construct object in-place
    return new (block->data) T(std::forward<Args>(args)...);
  }

  // Release an object back to the pool
  void release(T *obj) {
    if (!obj)
      return;

    std::lock_guard<std::mutex> lock(pool_mutex);

    // Call destructor
    obj->~T();

    // Return block to pool
    Block *block = reinterpret_cast<Block *>(obj);
    available_blocks.push(block);
    total_in_use--;
  }

  // Get pool statistics
  struct Statistics {
    size_t total_allocated;
    size_t total_in_use;
    size_t available;
    size_t chunks_allocated;
    double utilization_ratio;
  };

  Statistics getStatistics() const {
    std::lock_guard<std::mutex> lock(pool_mutex);
    return {total_allocated, total_in_use, available_blocks.size(),
            chunks.size(),
            total_allocated > 0 ? double(total_in_use) / total_allocated : 0.0};
  }

  // Clear all unused blocks (for memory optimization)
  void shrink() {
    std::lock_guard<std::mutex> lock(pool_mutex);
    // Keep only one chunk if possible
    // Implementation would depend on tracking which blocks belong to which
    // chunks
  }

private:
  void allocateNewChunk() {
    auto new_chunk = std::make_unique<Block[]>(chunk_size);

    // Link all blocks in the chunk
    for (size_t i = 0; i < chunk_size; ++i) {
      available_blocks.push(&new_chunk[i]);
    }

    total_allocated += chunk_size;
    chunks.push_back(std::move(new_chunk));
  }
};

// RAII wrapper for automatic memory pool management
template <typename T> class PooledObject {
private:
  T *obj;
  MemoryPool<T> *pool;

public:
  template <typename... Args>
  PooledObject(MemoryPool<T> &p, Args &&...args)
      : pool(&p), obj(pool->acquire(std::forward<Args>(args)...)) {}

  ~PooledObject() {
    if (obj && pool) {
      pool->release(obj);
    }
  }

  // Move constructor
  PooledObject(PooledObject &&other) noexcept
      : obj(other.obj), pool(other.pool) {
    other.obj = nullptr;
    other.pool = nullptr;
  }

  // Move assignment
  PooledObject &operator=(PooledObject &&other) noexcept {
    if (this != &other) {
      if (obj && pool) {
        pool->release(obj);
      }
      obj = other.obj;
      pool = other.pool;
      other.obj = nullptr;
      other.pool = nullptr;
    }
    return *this;
  }

  // Delete copy operations
  PooledObject(const PooledObject &) = delete;
  PooledObject &operator=(const PooledObject &) = delete;

  // Access operators
  T *operator->() { return obj; }
  const T *operator->() const { return obj; }
  T &operator*() { return *obj; }
  const T &operator*() const { return *obj; }
  T *get() { return obj; }
  const T *get() const { return obj; }

  // Check if valid
  explicit operator bool() const { return obj != nullptr; }
};

// Specialized memory pools for common TreeWidzard types
class TreeWidzardMemoryManager {
private:
  static std::unique_ptr<TreeWidzardMemoryManager> instance;
  static std::once_flag init_flag;

  MemoryPool<State> state_pool;
  MemoryPool<::WitnessSet> witness_set_pool;
  MemoryPool<::Bag> bag_pool;
  MemoryPool<ConjectureNode> conjecture_node_pool;

  TreeWidzardMemoryManager() = default;

public:
  static TreeWidzardMemoryManager &getInstance() {
    std::call_once(init_flag, []() {
      instance = std::unique_ptr<TreeWidzardMemoryManager>(
          new TreeWidzardMemoryManager());
    });
    return *instance;
  }

  // Factory methods for pooled objects
  template <typename... Args> PooledObject<State> createState(Args &&...args) {
    return PooledObject<State>(state_pool, std::forward<Args>(args)...);
  }

  template <typename... Args>
  PooledObject<WitnessSet> createWitnessSet(Args &&...args) {
    return PooledObject<WitnessSet>(witness_set_pool,
                                    std::forward<Args>(args)...);
  }

  template <typename... Args> PooledObject<Bag> createBag(Args &&...args) {
    return PooledObject<Bag>(bag_pool, std::forward<Args>(args)...);
  }

  template <typename... Args>
  PooledObject<ConjectureNode> createConjectureNode(Args &&...args) {
    return PooledObject<ConjectureNode>(conjecture_node_pool,
                                        std::forward<Args>(args)...);
  }

  // Get memory pool statistics
  struct PoolStatistics {
    MemoryPool<State>::Statistics state_stats;
    MemoryPool<WitnessSet>::Statistics witness_set_stats;
    MemoryPool<Bag>::Statistics bag_stats;
    MemoryPool<ConjectureNode>::Statistics conjecture_node_stats;
  };

  PoolStatistics getStatistics() const {
    return {state_pool.getStatistics(), witness_set_pool.getStatistics(),
            bag_pool.getStatistics(), conjecture_node_pool.getStatistics()};
  }

  // Memory cleanup
  void shrinkAllPools() {
    state_pool.shrink();
    witness_set_pool.shrink();
    bag_pool.shrink();
    conjecture_node_pool.shrink();
  }

  // Direct pool access for advanced use cases
  MemoryPool<State> &getStatePool() { return state_pool; }
  MemoryPool<WitnessSet> &getWitnessSetPool() { return witness_set_pool; }
  MemoryPool<Bag> &getBagPool() { return bag_pool; }
  MemoryPool<ConjectureNode> &getConjectureNodePool() {
    return conjecture_node_pool;
  }
};

// Convenience macros for pool usage
#define CREATE_POOLED_STATE(...)                                               \
  TreeWidzardMemoryManager::getInstance().createState(__VA_ARGS__)

#define CREATE_POOLED_WITNESS_SET(...)                                         \
  TreeWidzardMemoryManager::getInstance().createWitnessSet(__VA_ARGS__)

#define CREATE_POOLED_BAG(...)                                                 \
  TreeWidzardMemoryManager::getInstance().createBag(__VA_ARGS__)

#define CREATE_POOLED_CONJECTURE_NODE(...)                                     \
  TreeWidzardMemoryManager::getInstance().createConjectureNode(__VA_ARGS__)

} // namespace TreeWidzard

#endif // MEMORY_POOL_H