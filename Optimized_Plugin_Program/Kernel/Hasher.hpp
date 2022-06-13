#ifndef HASHER_FACTORY_H
#define HASHER_FACTORY_H

#include <cstdint>

class Hasher {
  uint64_t state;

 public:
  Hasher(uint64_t initial_state) : state(initial_state) {}
  
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
