// https://github.com/soryuusi0219/compro/blob/main/library/utility/64_ary_tree.hpp
#pragma once
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include "../math/internal/internal_bit.hpp"

namespace soryuusi {

using namespace std;

template<class T>
struct BitSetSet{
  using u64 = uint64_t;
  private:
    static constexpr int mask = 63;
    T n = 0;
    int h = 0;
    array<size_t,(numeric_limits<T>::digits+5)/6> offset{};
    vector<u64> data;

  public:
    BitSetSet() = default;
    BitSetSet(T n_) : n(n_){
      assert(n >= 0);
      size_t words = n, total = 0;
      while(words > 1 || h == 0){
        words = (words >> 6) + ((words & mask) != 0);
        offset[h++] = total;
        total += words+1;  // Zero sentinel at the end of each level.
      }
      data.resize(total);
    }
    void insert(T x){
      assert(0 <= x && x < n);
      for(int i = 0; i < h; ++i){
        u64 bit = u64(1) << (x & mask);
        u64& word = data[offset[i]+(x >>= 6)];
        u64 before = word;
        word |= bit;
        if(before) break;
      }
    }
    void erase(T x){
      assert(0 <= x && x < n);
      for(int i = 0; i < h; ++i){
        u64 bit = u64(1) << (x & mask);
        if(data[offset[i]+(x >>= 6)] &= ~bit) break;
      }
    }
    bool operator[](T x) const {
      assert(0 <= x && x < n);
      return (data[x >> 6] >> (x & mask)) & 1;
    }
    bool contains(T x){
      return operator[](x);
    }
    T next(T x){
      if(x < 0) x = 0;
      if(x >= n) return n;
      size_t index = x, leaf = index >> 6;
      if(u64 word = data[leaf] >> (index & mask)) return static_cast<T>(index+soryuusi::countr_zero(word));
      index = leaf+1;
      for(int i = 1; i < h; ++i){
        size_t block = index >> 6;
        u64 word = data[offset[i]+block] >> (index & mask);
        if(word){
          index += soryuusi::countr_zero(word);
          for(int j = i-1; j >= 0; --j)
            index = (index << 6) + soryuusi::countr_zero(data[offset[j]+index]);
          return static_cast<T>(index);
        }
        index = block+1;
      }
      return n;
    }
    T prev(T x){
      if(x > n) x = n;
      if(x <= 0) return T(-1);
      size_t index = x-1, leaf = index >> 6;
      if(u64 word = data[leaf] << (mask-(index & mask))) return static_cast<T>(index-soryuusi::countl_zero(word));
      if(leaf == 0) return T(-1);
      index = leaf-1;
      for(int i = 1; i < h; ++i){
        size_t block = index >> 6;
        u64 word = data[offset[i]+block] << (mask-(index & mask));
        if(word){
          index -= soryuusi::countl_zero(word);
          for(int j = i-1; j >= 0; --j)
            index = (index << 6) + mask-soryuusi::countl_zero(data[offset[j]+index]);
          return static_cast<T>(index);
        }
        if(block == 0) break;
        index = block-1;
      }
      return T(-1);
    }
};

}  // namespace soryuusi
