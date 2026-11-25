// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/static_rmq.hpp
#pragma once
#include <algorithm>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "../math/internal/internal_bit.hpp"

namespace soryuusi {

namespace internal {

template<class T, bool Maximum>
constexpr T static_rmq_identity(){
  static_assert(std::numeric_limits<T>::is_specialized, "provide an identity for this value type");
  if constexpr (std::numeric_limits<T>::has_infinity){
    return Maximum ? -std::numeric_limits<T>::infinity() : std::numeric_limits<T>::infinity();
  }else{
    return Maximum ? std::numeric_limits<T>::lowest() : std::numeric_limits<T>::max();
  }
}

}  // namespace internal

template<class T, bool Maximum = false, int BlockSize = 32, auto identity = internal::static_rmq_identity<T,Maximum>>
struct StaticRMQ{
  static_assert(BlockSize == 0 || (BlockSize > 0 && BlockSize <= 64 && soryuusi::has_single_bit(static_cast<unsigned>(BlockSize))));

  static constexpr int B = BlockSize;
  static constexpr int b = B ? soryuusi::countr_zero(static_cast<unsigned>(B)) : 0;
  static constexpr T inf = identity();
  int n = 0, m = 0, h = 0;
  std::vector<T> data;

  StaticRMQ() = default;
  StaticRMQ(std::vector<T> values){build(std::move(values));}

  void build(std::vector<T> values){
    data = std::move(values);
    n = data.size();
    if constexpr (B == 0){
      const unsigned log = soryuusi::bit_width(static_cast<unsigned>(n));
      shift = log ? soryuusi::bit_width(log)-1 : 0;
    }
    const int width = 1<<block_shift();
    m = n ? (n-1)/width+1 : 0;
    h = m ? soryuusi::bit_width(static_cast<unsigned>(m))-1 : 0;
    blocks.clear();
    if(!n){masks.clear(); return;}
    blocks.reserve(h+1);
    if constexpr (B == 1){
      blocks.emplace_back();
    }else{
      masks.resize(n);
      auto& base = blocks.emplace_back(m);
      for(int k = 0; k < m; ++k){
        const int start = k*width, count = std::min(width,n-start);
        int stack[B ? B : 32];
        int top = 0;
        Word candidates = 0;
        for(int i = 0; i < count; ++i){
          const T value = data[start+i];
          while(top && better(value,data[start+stack[top-1]])) candidates ^= Word{1}<<stack[--top];
          stack[top++] = i;
          candidates |= Word{1}<<i;
          masks[start+i] = candidates;
        }
        base[k] = data[start+soryuusi::countr_zero(candidates)];
      }
    }
    for(int level = 1; level <= h; ++level){
      const auto& prev = level == 1 && B == 1 ? data : blocks[level-1];
      auto& row = blocks.emplace_back(m-(1<<level)+1);
      for(int i = 0; i < (int)row.size(); ++i) row[i] = extremum(prev[i],prev[i+(1<<(level-1))]);
    }
  }

  T naive_prod(int l, int r) const {
    if(l >= r) return inf;
    T result = data[l];
    for(int i = l+1; i < r; ++i) result = extremum(result,data[i]);
    return result;
  }

  T prod(int l, int r) const {
    if(l >= r) return inf;
    --r;
    if constexpr (B == 1){
      return block_prod(l,r);
    }else{
      const int bits = block_shift();
      const int left = l>>bits, right = r>>bits;
      if(left == right) return within_block(l,r,left,bits);
      const auto edges = [&](){
        return extremum(within_block(l,((left+1)<<bits)-1,left,bits),within_block(right<<bits,r,right,bits));
      };
      if(left+1 == right) return edges();
      const T middle = block_prod(left+1,right-1);
      // Full boundary blocks bound every possible extremum of their partial ranges.
      if(!better(extremum(blocks[0][left],blocks[0][right]),middle)) return middle;
      return extremum(middle,edges());
    }
  }

private:
  using Word = std::conditional_t<B == 64,std::uint64_t,std::uint32_t>;
  using Mask = std::conditional_t<B == 0 || B >= 32,Word, std::conditional_t<(B<=8),std::uint8_t,std::uint16_t>>;
  std::vector<Mask> masks;
  std::vector<std::vector<T>> blocks;
  int shift = 0;

  static bool better(const T& x, const T& y){
    if constexpr (Maximum) return y < x;
    else return x < y;
  }

  static T extremum(const T& x, const T& y){return better(y,x) ? y : x;}
  int block_shift() const {return B == 0 ? shift : b;}

  T within_block(int l, int r, int block, int bits) const {
    const int start = block<<bits;
    const Word candidates = masks[r] & (~Word{0} << (l-start));
    return data[start+soryuusi::countr_zero(candidates)];
  }

  T block_prod(int l, int r) const {
    const int level = soryuusi::bit_width(static_cast<unsigned>(r-l+1))-1;
    if constexpr (B == 1) if(level == 0) return data[l];
    const auto& row = blocks[level];
    return extremum(row[l],row[r-(1<<level)+1]);
  }
};

}  // namespace soryuusi
