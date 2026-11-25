// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/static_range_prod.hpp
#pragma once
#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

#include "../math/internal/internal_bit.hpp"

namespace soryuusi {

template<class S, auto op, auto e, int BlockSize = 8>
struct StaticRangeProd{
  static_assert(std::is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(std::is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");
  static_assert(BlockSize > 0 && soryuusi::has_single_bit(static_cast<unsigned>(BlockSize)));

  static constexpr int B = BlockSize;
  static constexpr int b = soryuusi::countr_zero(static_cast<unsigned>(B));
  int n = 0, m = 0, h = 0;
  std::vector<S> data;

  StaticRangeProd(std::vector<S> v) : n(v.size()), m(n ? (n-1)/B+1 : 0),
      h(m ? soryuusi::bit_width(static_cast<unsigned>(m-1)) : 0), data(std::move(v)){
    if(m <= 1) return;
    if constexpr (B == 1){
      build_table(data);
    }else{
      prefix.assign(n,e());
      suffix.assign(n,e());
      std::vector<S> blocks(m,e());
      for(int k = 0; k < m; ++k){
        const int l = k*B, r = l+std::min(B,n-l);
        prefix[l] = data[l];
        for(int i = l+1; i < r; ++i) prefix[i] = op(prefix[i-1],data[i]);
        blocks[k] = prefix[r-1];
        suffix[r-1] = data[r-1];
        for(int i = r-2; i >= l; --i) suffix[i] = op(data[i],suffix[i+1]);
      }
      build_table(blocks);
    }
  }

  S naive_prod(int l, int r) const {
    if(l >= r) return e();
    S res = data[l];
    for(int i = l+1; i < r; ++i) res = op(res,data[i]);
    return res;
  }

  S prod(int l, int r) const {
    if(l >= r) return e();
    --r;
    if constexpr (B == 1){
      return l == r ? data[l] : block_prod(l,r);
    }else{
      const int left = l/B, right = r/B;
      if(left == right) return naive_prod(l,r+1);
      if(left+1 == right) return op(suffix[l],prefix[r]);
      const S middle = left+2 == right ? prefix[right*B-1] : block_prod(left+1,right-1);
      return op(op(suffix[l],middle),prefix[r]);
    }
  }

private:
  std::vector<S> prefix, suffix, table;

  void build_table(const std::vector<S>& blocks){
    table.assign(std::size_t(h)*m,e());
    for(int level = 0; level < h; ++level){
      const std::size_t row = std::size_t(level)*m;
      const int half = 1<<level;
      for(int start = 0; start < m-half;){
        const int mid = start+half, end = mid+std::min(half,m-mid);
        table[row+mid-1] = blocks[mid-1];
        for(int j = mid-2; j >= start; --j) table[row+j] = op(blocks[j],table[row+j+1]);
        table[row+mid] = blocks[mid];
        for(int j = mid+1; j < end; ++j) table[row+j] = op(table[row+j-1],blocks[j]);
        start = end;
      }
    }
  }

  S block_prod(int l, int r) const {
    const int level = soryuusi::bit_width(static_cast<unsigned>(l^r))-1;
    const std::size_t row = std::size_t(level)*m;
    return op(table[row+l],table[row+r]);
  }
};

}  // namespace soryuusi
