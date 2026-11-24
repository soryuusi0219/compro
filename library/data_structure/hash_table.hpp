// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/hash_table.hpp
#pragma once
#include <chrono>
#include <cstdint>
#include <type_traits>

#include "internal/hash_table_storage.hpp"

namespace soryuusi {

using namespace std;

template<class T, class U, auto hash>
struct HashTable{
  static_assert(is_invocable_r_v<uint64_t, decltype(hash), T>, "hash must be callable with (T) and return a value convertible to uint64_t");

  using u32 = uint32_t;
  using u64 = uint64_t;
  private:
    internal::HashTableStorage<T,U,hash> storage;
  public:
    HashTable(int n) : storage(n) {}
    U &operator[](const T &k) {
      return storage.value(storage.insert(k,[]{ return U{}; }));
    }
    bool contains(const T &k){
      return storage.lookup(k) != storage.end();
    }
    U get(const T &k, U defalt) {
      auto i = storage.lookup(k);
      return i == storage.end() ? defalt : storage.value(i);
    }
};
template<class T>
uint64_t hash_int(T x){
  static const uint64_t R = []() -> uint64_t {
    uint64_t x = chrono::steady_clock::now().time_since_epoch().count();
    for(int i = 0; i < 8; i++) {
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
    }
    return x;
  }();
  uint64_t res = static_cast<uint64_t>(x);
  res += R;
  res = (res ^ (res >> 30)) * 0xbf58476d1ce4e5b9;
  res = (res ^ (res >> 27)) * 0x94d049bb133111eb;
  return (res ^ (res >> 31));
}
// template<int M> uint64_t hash_mint(static_modint<M> x){ return hash_int(x.val()); }
// template<int id> uint64_t hash_mint(dynamic_modint<id> x){ return hash_int(x.val()); }

}  // namespace soryuusi
