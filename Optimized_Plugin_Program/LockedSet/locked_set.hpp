#include <functional>
#include <mutex>
#include <set>
#include <shared_mutex>

/**
 * @brief Set that automatically locks a mutex when used.
 * Allows multiple threads to read at once, but only one to write.
 */
template <class Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key>>
class locked_set : std::set<Key, Compare, Allocator> {
  using base = std::set<Key, Compare, Allocator>;
  mutable std::shared_mutex mutex;

  template <template <class> class Lock, class Iterator>
  class iterator_wrapper : public Iterator {
    Lock<std::shared_mutex> lock_;

   public:
    iterator_wrapper(Lock<std::shared_mutex> &&lock, Iterator &&iterator)
        : lock_(std::move(lock)), Iterator(std::move(iterator)) {}
  };

 public:
  // non const iterator holds a unique lock for its entire lifetime
  using iterator = iterator_wrapper<std::unique_lock, typename base::iterator>;
  // const iterator holds a shared lock for its entire lifetime
  using const_iterator =
      iterator_wrapper<std::shared_lock, typename base::const_iterator>;

  // Inherit public constructor
  using base::base;

  decltype(auto) size() const {
    std::shared_lock lock(mutex);
    return base::size();
  }

  decltype(auto) empty() const {
    std::shared_lock lock(mutex);
    return base::empty();
  }

  decltype(auto) count(const Key &value) const {
    std::shared_lock lock(mutex);
    return base::count(value);
  }

  decltype(auto) clear() {
    std::unique_lock lock(mutex);
    return base::clear();
  }

  // TODO return something sensible
  template <class K>
  void insert(K &&value) {
    // don't lock if the value is already there
    if (count(value)) return;

    std::unique_lock lock(mutex);
    base::insert(std::forward<K>(value));
  }

  /*
  template <class InputIt>
  void insert(InputIt first, InputIt last) {
    std::unique_lock lock(mutex);
    base::insert(first, last);
  }
  //*/

  template <template <class> class Lock, class Iterator>
  void insert(const iterator_wrapper<Lock, Iterator> &first,
              const iterator_wrapper<Lock, Iterator> &last) {
    std::unique_lock lock(mutex);
    base::insert(Iterator(first), Iterator(last));
  }

  iterator begin() {
    std::unique_lock lock(mutex);
    return {std::move(lock), base::begin()};
  }

  iterator end() {
    std::unique_lock lock(mutex);
    return {std::move(lock), base::end()};
  }

  const_iterator cbegin() const {
    std::shared_lock lock(mutex);
    return {std::move(lock), base::cbegin()};
  }

  const_iterator cend() const {
    std::shared_lock lock(mutex);
    return {std::move(lock), base::cend()};
  }

  const_iterator begin() const { return cbegin(); }
  const_iterator end() const { return cend(); }

};
