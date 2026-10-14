// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dynamic_dsu.hpp
#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

struct dynamic_dsu{
  private:
    struct HashTable{
      vector<int> key;
      vector<int> val;
      vector<bool> used;
      int mask;
      int hash(int x){
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
      int index(int x){
        static const int R = []() -> uint64_t {
          uint64_t x = chrono::steady_clock::now().time_since_epoch().count();
          for(int i = 0; i < 8; i++) {
              x ^= x << 13;
              x ^= x >> 7;
              x ^= x << 17;
          }
          return x | 1;
        }() & mask;
        int i = hash(x) & mask;
        for(; used[i] && key[i] != x; i = (i+R) & mask);
        if(!used[i]) used[i] = 1, key[i] = x, val[i] = -1;
        return i;
      }
      HashTable(int n){
        int s = 1; while(s < n * 2) s <<= 1;
        key.resize(s);
        val.resize(s);
        used.resize(s);
        mask = s-1;
      }
      int &operator[](int k) {return val[index(k)];}
    } e;
  public:
    dynamic_dsu(int n) : e(n) {}
    int leader(int u) { return e[u] < 0 ? u : e[u] = leader(e[u]); }
    bool same(int u, int v) { return leader(u) == leader(v);}
    int size(int u) { return -e[leader(u)]; }
    int merge(int u, int v) {
      u = leader(u), v = leader(v);
      if(u == v) return u;
      int i = e.index(u), j = e.index(v);
      if(e.val[i] > e.val[j]) swap(u, v), swap(i, j);
      e.val[i] += e.val[j];
      e.val[j] = u;
      return u;
    }
};

}  // namespace soryuusi
