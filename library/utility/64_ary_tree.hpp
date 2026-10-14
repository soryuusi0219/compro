// https://github.com/soryuusi0219/compro/blob/main/library/utility/64_ary_tree.hpp
#pragma once
#include <cassert>
#include <cstdint>
#include <iterator>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
struct BitSetSet{
  using u64 = uint64_t;
  private:
    static constexpr int mask = 0x3f;
    T n;
    int h;
    vector<vector<u64>> data;
  public:
    BitSetSet() = default;
    BitSetSet(T n_) : n(n_), h(0){
      do data.push_back(vector<u64>(n_ = (n_+mask) >> 6,0));
      while(n_ > 1);
      h = data.size();
    }
    void insert(T x){
      assert(0 <= x && x < n);
      for(int i = 0; i < h; i++) {
        int xl = (x & mask);
        data[i][x >>= 6] |= 1ull << xl;
      }
    }
    void erase(T x){
      assert(0 <= x && x < n);
      for(int i = 0; i < h; i++) {
        int xl = (x & mask);
        if(data[i][x >>= 6] &= ~(1ull << xl)) break;
      }
    }
    bool operator[](T x) const {
      assert(0 <= x && x < n);
      return (data[0][x >> 6] >> (x & mask)) & 1;
    }
    bool contains(T x){
      return operator[](x);
    }
    T next(T x){
      for(int i = 0; i < h; i++){
        if(x >> 6 >= (T)data[i].size()) break;
        u64 y = data[i][x >> 6] >> (x & mask);
        if(!y){
          x = (x >> 6) + 1;
          continue;
        }
        x += __builtin_ctzll(y);
        for(int j = i-1; j >= 0; j--){
          x <<= 6;
          x += __builtin_ctzll(data[j][x >> 6]);
        }
        return x;
      }
      return n;
    }
    T prev(T x){
      --x;
      for(int i = 0; i < h; i++){
        if(x < 0) break;
        u64 y = data[i][x >> 6] << (~x & mask);
        if(!y){
          x = (x >> 6) - 1;
          continue;
        }
        x -= __builtin_clzll(y);
        for(int j = i-1; j >= 0; j--){
          x <<= 6;
          x += mask - __builtin_clzll(data[j][x >> 6]);
        }
        return x;
      }
      return -1;
    }
};

}  // namespace soryuusi
