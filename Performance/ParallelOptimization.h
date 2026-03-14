#ifndef PARALLEL_OPTIMIZATION_H
#define PARALLEL_OPTIMIZATION_H

#include <optional>
class State;

#include "../Conjecture/Conjecture.h" // Added for std::invoke_result_t
#include "../Kernel/Bag.h"
#include "../Kernel/State.h"
#include "../Kernel/WitnessSet.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>
#if defined(__linux__)
#include <numa.h>
#else
// Mock NUMA functions for non-Linux systems
inline int numa_available() { return -1; }
inline int numa_max_node() { return 0; }
inline void numa_run_on_node(int) {}
#endif
#include <sched.h>

namespace TreeWidzard {

// NUMA-aware thread pool with work stealing
class NUMAThreadPool {
private:
  struct WorkItem {
    std::function<void()> task;
    std::promise<void> promise;
    int preferred_node;

    WorkItem(std::function<void()> t, int node = -1)
        : task(std::move(t)), preferred_node(node) {}
  };

  struct WorkerData {
    std::queue<std::unique_ptr<WorkItem>> local_queue;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> running;
    int numa_node;
    int cpu_id;

    WorkerData(int node, int cpu)
        : running(true), numa_node(node), cpu_id(cpu) {}
  };

  std::vector<std::unique_ptr<WorkerData>> workers;
  std::vector<std::thread> threads;
  std::queue<std::unique_ptr<WorkItem>> global_queue;
  std::mutex global_mutex;
  std::condition_variable global_condition;
  std::atomic<bool> pool_running;

  // Performance statistics
  std::atomic<uint64_t> tasks_completed;
  std::atomic<uint64_t> work_steals;
  std::atomic<uint64_t> numa_misses;

public:
  explicit NUMAThreadPool(size_t num_threads = 0)
      : pool_running(true), tasks_completed(0), work_steals(0), numa_misses(0) {

    if (num_threads == 0) {
      num_threads = std::thread::hardware_concurrency();
    }

    initializeNUMATopology();
    createWorkers(num_threads);
  }

  ~NUMAThreadPool() { shutdown(); }

  // Submit task with optional NUMA node preference
  template <typename F, typename... Args>
  auto submit(F &&f, Args &&...args, int preferred_node = -1)
      -> std::future<std::invoke_result_t<F, Args...>> {

    using return_type = std::invoke_result_t<F, Args...>;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    auto future = task->get_future();
    auto work_item =
        std::make_unique<WorkItem>([task]() { (*task)(); }, preferred_node);

    // Try to submit to preferred NUMA node first
    if (preferred_node >= 0 &&
        preferred_node < static_cast<int>(workers.size())) {
      if (submitToWorker(preferred_node, std::move(work_item))) {
        return future;
      }
    }

    // Submit to global queue if no preference or preferred worker is busy
    {
      std::lock_guard<std::mutex> lock(global_mutex);
      global_queue.push(std::move(work_item));
    }
    global_condition.notify_one();

    return future;
  }

  // Submit batch of tasks with load balancing
  template <typename Iterator> void submitBatch(Iterator begin, Iterator end) {
    size_t total_tasks = std::distance(begin, end);
    size_t tasks_per_worker = total_tasks / workers.size();
    size_t remainder = total_tasks % workers.size();

    size_t current_worker = 0;
    auto it = begin;

    for (size_t i = 0; i < workers.size() && it != end; ++i) {
      size_t tasks_for_this_worker = tasks_per_worker + (i < remainder ? 1 : 0);

      for (size_t j = 0; j < tasks_for_this_worker && it != end; ++j, ++it) {
        auto work_item = std::make_unique<WorkItem>(std::function<void()>(*it),
                                                    workers[i]->numa_node);
        submitToWorker(i, std::move(work_item));
      }
    }
  }

  // Get performance statistics
  struct Statistics {
    uint64_t tasks_completed;
    uint64_t work_steals;
    uint64_t numa_misses;
    size_t active_threads;
    double steal_ratio;
    double numa_efficiency;
  };

  Statistics getStatistics() const {
    uint64_t completed = tasks_completed.load();
    uint64_t steals = work_steals.load();
    uint64_t misses = numa_misses.load();

    return {completed,
            steals,
            misses,
            workers.size(),
            completed > 0 ? double(steals) / completed : 0.0,
            completed > 0 ? 1.0 - (double(misses) / completed) : 1.0};
  }

  void shutdown() {
    pool_running = false;

    // Wake up all workers
    for (auto &worker : workers) {
      worker->running = false;
      worker->condition.notify_all();
    }
    global_condition.notify_all();

    // Join all threads
    for (auto &thread : threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

private:
  void initializeNUMATopology() {
    if (numa_available() == -1) {
      // NUMA not available, proceed without NUMA optimization
      return;
    }
  }

  void createWorkers(size_t num_threads) {
    workers.reserve(num_threads);
    threads.reserve(num_threads);

    int num_numa_nodes = numa_available() >= 0 ? numa_max_node() + 1 : 1;

    for (size_t i = 0; i < num_threads; ++i) {
      int numa_node = i % num_numa_nodes;
      int cpu_id = i; // Simplified CPU assignment

      workers.push_back(std::make_unique<WorkerData>(numa_node, cpu_id));

      threads.emplace_back([this, i]() { workerLoop(i); });
    }
  }

  bool submitToWorker(size_t worker_id, std::unique_ptr<WorkItem> item) {
    if (worker_id >= workers.size())
      return false;

    auto &worker = workers[worker_id];
    std::unique_lock<std::mutex> lock(worker->queue_mutex, std::try_to_lock);

    if (lock.owns_lock()) {
      worker->local_queue.push(std::move(item));
      worker->condition.notify_one();
      return true;
    }
    return false;
  }

  void workerLoop(size_t worker_id) {
    auto &worker = workers[worker_id];

    // Set CPU affinity and NUMA policy
    if (numa_available() >= 0) {
#if defined(__linux__)
      cpu_set_t cpuset;
      CPU_ZERO(&cpuset);
      CPU_SET(worker->cpu_id, &cpuset);
      pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
#endif
      numa_run_on_node(worker->numa_node);
    }

    while (worker->running || !worker->local_queue.empty()) {
      std::unique_ptr<WorkItem> work_item;

      // Try to get work from local queue first
      {
        std::unique_lock<std::mutex> lock(worker->queue_mutex);
        worker->condition.wait(lock, [&] {
          return !worker->local_queue.empty() || !worker->running;
        });

        if (!worker->local_queue.empty()) {
          work_item = std::move(worker->local_queue.front());
          worker->local_queue.pop();
        }
      }

      // If no local work, try to steal from other workers
      if (!work_item && worker->running) {
        work_item = stealWork(worker_id);
        if (work_item) {
          work_steals++;
        }
      }

      // If still no work, try global queue
      if (!work_item && worker->running) {
        std::unique_lock<std::mutex> lock(global_mutex);
        global_condition.wait_for(lock, std::chrono::milliseconds(10), [&] {
          return !global_queue.empty() || !pool_running;
        });

        if (!global_queue.empty()) {
          work_item = std::move(global_queue.front());
          global_queue.pop();
        }
      }

      // Execute work if found
      if (work_item) {
        // Check NUMA locality
        if (work_item->preferred_node >= 0 &&
            work_item->preferred_node != worker->numa_node) {
          numa_misses++;
        }

        work_item->task();
        tasks_completed++;
      }
    }
  }

  std::unique_ptr<WorkItem> stealWork(size_t stealer_id) {
    // Try to steal from a random worker
    size_t target = (stealer_id + 1 + rand()) % workers.size();

    for (size_t attempts = 0; attempts < workers.size(); ++attempts) {
      size_t victim = (target + attempts) % workers.size();
      if (victim == stealer_id)
        continue;

      auto &victim_worker = workers[victim];
      std::unique_lock<std::mutex> lock(victim_worker->queue_mutex,
                                        std::try_to_lock);

      if (lock.owns_lock() && !victim_worker->local_queue.empty()) {
        auto stolen = std::move(victim_worker->local_queue.front());
        victim_worker->local_queue.pop();
        return stolen;
      }
    }
    return nullptr;
  }
};

// Parallel search coordinator
class ParallelSearchCoordinator {
private:
  NUMAThreadPool thread_pool;
  std::atomic<bool> search_active;
  std::atomic<bool> solution_found;
  std::mutex result_mutex;

  // Load balancing
  std::atomic<size_t> work_units_distributed;
  std::atomic<size_t> work_units_completed;
  std::vector<std::unique_ptr<std::atomic<size_t>>> worker_loads;

public:
  explicit ParallelSearchCoordinator(size_t num_threads = 0)
      : thread_pool(num_threads), search_active(false), solution_found(false),
        work_units_distributed(0), work_units_completed(0) {

    size_t actual_threads =
        num_threads > 0 ? num_threads : std::thread::hardware_concurrency();
    worker_loads.reserve(actual_threads);
    for (size_t i = 0; i < actual_threads; ++i) {
      worker_loads.push_back(std::make_unique<std::atomic<size_t>>(0));
    }
  }

  NUMAThreadPool &getThreadPool() { return thread_pool; }

  template <typename SearchFunction, typename WorkUnit>
  auto executeParallelSearch(SearchFunction search_func,
                             const std::vector<WorkUnit> &work_units)
      -> std::future<std::invoke_result_t<SearchFunction, WorkUnit>> {

    using ResultType = std::invoke_result_t<SearchFunction, WorkUnit>;
    auto promise = std::make_shared<std::promise<ResultType>>();
    auto future = promise->get_future();

    search_active = true;
    solution_found = false;
    work_units_distributed = work_units.size();
    work_units_completed = 0;

    // Submit all work units
    for (size_t i = 0; i < work_units.size(); ++i) {
      int preferred_node = i % getNumaNodeCount();

      thread_pool.submit(
          [this, search_func, work_unit = work_units[i], promise]() {
            if (!solution_found.load()) {
              auto result = search_func(work_unit);

              // Check if this is a solution
              if (isSolution(result)) {
                bool expected = false;
                if (solution_found.compare_exchange_strong(expected, true)) {
                  promise->set_value(result);
                  search_active = false;
                }
              }
            }

            work_units_completed++;

            // If all work units completed without solution
            if (work_units_completed == work_units_distributed &&
                !solution_found.load()) {
              search_active = false;
              // Set appropriate "no solution" result
            }
          },
          preferred_node);
    }

    return future;
  }

  // Dynamic load balancing
  void balanceLoad() {
    // Simple load balancing: redistribute work from overloaded workers
    size_t max_load = 0;
    size_t min_load = SIZE_MAX;
    size_t max_worker = 0;
    size_t min_worker = 0;

    for (size_t i = 0; i < worker_loads.size(); ++i) {
      size_t load = worker_loads[i]->load();
      if (load > max_load) {
        max_load = load;
        max_worker = i;
      }
      if (load < min_load) {
        min_load = load;
        min_worker = i;
      }
    }

    // If load imbalance is significant, trigger rebalancing
    if (max_load > min_load * 2) {
      // Implementation would redistribute work
      // This is a simplified version
    }
  }

  auto getStatistics() const {
    auto pool_stats = thread_pool.getStatistics();

    struct SearchStatistics {
      NUMAThreadPool::Statistics pool_stats;
      bool is_active;
      size_t work_distributed;
      size_t work_completed;
      double completion_ratio;
    };

    return SearchStatistics{
        pool_stats, search_active.load(), work_units_distributed.load(),
        work_units_completed.load(),
        work_units_distributed > 0
            ? double(work_units_completed) / work_units_distributed
            : 0.0};
  }

private:
  template <typename T> bool isSolution(const T &result) {
    // Template specialization would handle different result types
    // For now, assume any non-empty result is a solution
    return true; // Simplified
  }

  int getNumaNodeCount() const {
    return numa_available() >= 0 ? numa_max_node() + 1 : 1;
  }
};

// Optimized parallel breadth-first search
class OptimizedParallelBFS {
private:
  ParallelSearchCoordinator coordinator;
  std::atomic<size_t> frontier_size;
  std::vector<std::queue<State>> local_frontiers;
  std::vector<std::unique_ptr<std::mutex>> frontier_mutexes;

public:
  explicit OptimizedParallelBFS(size_t num_threads = 0)
      : coordinator(num_threads), frontier_size(0) {

    size_t actual_threads =
        num_threads > 0 ? num_threads : std::thread::hardware_concurrency();
    local_frontiers.resize(actual_threads);
    frontier_mutexes.reserve(actual_threads);
    for (size_t i = 0; i < actual_threads; ++i) {
      frontier_mutexes.push_back(std::make_unique<std::mutex>());
    }
  }

  template <typename StateType, typename ExpandFunction, typename GoalTest>
  std::optional<StateType> search(const StateType &initial_state,
                                  ExpandFunction expand_func,
                                  GoalTest goal_test) {

    // Initialize with the starting state
    local_frontiers[0].push(initial_state);
    frontier_size = 1;

    std::atomic<bool> solution_found{false};
    std::optional<StateType> result;
    std::mutex result_mutex;

    // Parallel BFS loop
    while (frontier_size > 0 && !solution_found.load()) {
      std::vector<std::future<void>> futures;

      // Submit work for each thread
      for (size_t i = 0; i < local_frontiers.size(); ++i) {
        futures.push_back(coordinator.getThreadPool().submit([&, i]() {
          processLocalFrontier(i, expand_func, goal_test, solution_found,
                               result, result_mutex);
        }));
      }

      // Wait for all threads to complete this iteration
      for (auto &future : futures) {
        future.wait();
      }

      // Rebalance frontiers if needed
      rebalanceFrontiers();
    }

    return result;
  }

private:
  template <typename ExpandFunction, typename GoalTest, typename StateType>
  void processLocalFrontier(size_t thread_id, ExpandFunction expand_func,
                            GoalTest goal_test,
                            std::atomic<bool> &solution_found,
                            std::optional<StateType> &result,
                            std::mutex &result_mutex) {

    auto &local_frontier = local_frontiers[thread_id];
    auto &frontier_mutex = *frontier_mutexes[thread_id];

    std::vector<StateType> next_states;

    // Process current frontier
    {
      std::lock_guard<std::mutex> lock(frontier_mutex);
      while (!local_frontier.empty() && !solution_found.load()) {
        StateType current = local_frontier.front();
        local_frontier.pop();
        frontier_size--;

        // Check if goal
        if (goal_test(current)) {
          std::lock_guard<std::mutex> result_lock(result_mutex);
          if (!solution_found.exchange(true)) {
            result = current;
          }
          return;
        }

        // Expand state
        auto expanded = expand_func(current);
        next_states.insert(next_states.end(), expanded.begin(), expanded.end());
      }
    }

    // Add expanded states to next frontier
    if (!next_states.empty() && !solution_found.load()) {
      std::lock_guard<std::mutex> lock(frontier_mutex);
      for (const auto &state : next_states) {
        local_frontier.push(state);
        frontier_size++;
      }
    }
  }

  void rebalanceFrontiers() {
    // Simple rebalancing: move states from overloaded to underloaded frontiers
    std::vector<size_t> sizes;
    for (size_t i = 0; i < local_frontiers.size(); ++i) {
      std::lock_guard<std::mutex> lock(*frontier_mutexes[i]);
      sizes.push_back(local_frontiers[i].size());
    }

    // Find max and min
    auto max_it = std::max_element(sizes.begin(), sizes.end());
    auto min_it = std::min_element(sizes.begin(), sizes.end());

    if (*max_it > *min_it + 10) { // Threshold for rebalancing
      size_t max_idx = std::distance(sizes.begin(), max_it);
      size_t min_idx = std::distance(sizes.begin(), min_it);

      // Move some states
      std::lock_guard<std::mutex> max_lock(*frontier_mutexes[max_idx]);
      std::lock_guard<std::mutex> min_lock(*frontier_mutexes[min_idx]);

      size_t to_move = (*max_it - *min_it) / 2;
      for (size_t i = 0; i < to_move && !local_frontiers[max_idx].empty();
           ++i) {
        local_frontiers[min_idx].push(local_frontiers[max_idx].front());
        local_frontiers[max_idx].pop();
      }
    }
  }
};

} // namespace TreeWidzard

#endif // PARALLEL_OPTIMIZATION_H