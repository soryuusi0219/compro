// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/hash_table.hpp
#pragma once
#include <chrono>
#include <cstdint>
#include <type_traits>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, class U, auto hash>
struct HashTable{
  static_assert(is_invocable_r_v<uint64_t, decltype(hash), T>, "hash must be callable with (T) and return a value convertible to uint64_t");

  using u32 = uint32_t;
  using u64 = uint64_t;
  private:
    vector<T> key;
    vector<U> val;
    vector<bool> used;
    u32 mask;
    u32 index(T x){
      static const u32 R = []() -> u64 {
        u64 x = chrono::steady_clock::now().time_since_epoch().count();
        for(int i = 0; i < 8; i++) {
          	x ^= x << 13;
	          x ^= x >> 7;
	          x ^= x << 17;
        }
        return x | 1;
      }() & mask;
      u64 i = hash(x) & mask;
      for(; used[i] && key[i] != x; i = (i+R) & mask);
      return i;
    }
  public:
    HashTable(int n){
      int s = 1; while(s < n * 2) s <<= 1;
      key.resize(s);
      val.resize(s);
      used.resize(s);
      mask = s-1;
    }
    U &operator[](const T &k) {
      u32 i = index(k);
      if(!used[i]) used[i] = 1, key[i] = k, val[i] = U{};
      return val[i];
    }
    bool contains(const T &k){
      return used[index(k)];
    }
    U get(const T &k, U defalt) {
      u32 i = index(k);
      return used[i] ? val[i] : defalt;
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
