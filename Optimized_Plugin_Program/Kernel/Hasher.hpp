#ifndef HASHER_H
#define HASHER_H

#include <cstdint>

/**
 * @brief Hasher is used to build hashes
 * using a stream of tokens that are hashable
 * using the std::hash function object, for example
 * std::string, int, char.
 * 
 * Usage:
 * 
 * Consider a struct containing
 * these fields:
 * ```
 * struct S {
 *  std::string s;
 *  int x;
 *  char c;
 * };
 * ```
 * 
 * If equality in this struct is
 * defined as element-wise equality,
 * then a suitable hash is a combination
 * of hashes of its fields.
 * 
 * ```
 * uint64_t hash_s(const S &obj) {
 *   Hasher h(some_seed);
 *   h << obj.s;
 *   // like with cout, tokens supplying tokens can be chained
 *   h << obj.x << obj.c;
 *   return h.get();
 * }
 * ```
 * 
 * For objects considered equal,
 * the stream of tokens should
 * supplied to the hasher should
 * also be the same, in the same order.
 * 
 * For objects considered unequal,
 * the token streams should be unequal,
 * or at least the probability that they
 * are equal should be low.
 * 
 * To allow more types to be usable as tokens with
 * the << operator on the hasher, extend
 * std with a specialization of std::hash like this:
 * 
 * ```
 * namespace std {
 *   template<>
 *   struct hash<MyType> {
 *     uint64_t operator()(const MyType &obj) const {
 *       // implementation of hash of the obj here
 *     }
 *   }
 * }  // namespace std
 * ```
 */
class Hasher {
  uint64_t state;

 public:
  Hasher(uint64_t initial_state) : state(initial_state) {}
  
  /**
   * @brief Add another hashable token to the stream.
   * Doesn't increase the size of the Hasher object,
   * and only advances the internal 64 bit state.
   * 
   * @tparam T the type of the token
   * @param x  token to include in the combined hash
   * @return   Hasher& reference to self, to allow chaining
   */
  template<class T>
  Hasher &operator<<(const T &x) {
    state = splitmix64(std::hash<T>{}(x) + state);
    return *this;
  }

  uint64_t get() const { return state; }

 private:
  static uint64_t splitmix64(uint64_t x) {
    // http://xorshift.di.unimi.it/splitmix64.c
    x += 0x9e3779b97f4a7c15;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
    return x ^ (x >> 31);
  }
};

#endif
