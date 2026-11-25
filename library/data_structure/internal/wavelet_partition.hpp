// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/wavelet_partition.hpp
#pragma once
#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <vector>

#include "../../math/internal/internal_bit.hpp"
#include "../static_bit_vector.hpp"

namespace soryuusi::internal {

template<class T>
class WaveletPartition {
  std::vector<T> buffer;
  std::uint64_t any = 0, all = ~std::uint64_t{0};

public:
  explicit WaveletPartition(const std::vector<T>& keys) {
    if constexpr(std::is_integral_v<T> && sizeof(T) <= sizeof(std::uint64_t)) {
      for(T x : keys) {
        any |= static_cast<std::uint64_t>(x);
        all &= static_cast<std::uint64_t>(x);
      }
    }
  }

  // For mixed levels, move both partitions in input order and pass the
  // original indices to the attached payload. Uniform levels keep their order.
  int split(std::vector<T>& keys, int bit, StaticBitVector& rank, auto take) {
    int n = keys.size();
    rank.build(n);
    if(n == 0) return 0;
    if constexpr(std::is_integral_v<T> && sizeof(T) <= sizeof(std::uint64_t)) {
      if(bit < 64) {
        if(!((any >> bit) & 1)) return n;
        if((all >> bit) & 1) {
          int words = n >> 6;
          std::fill_n(rank.bits.begin(),words,~StaticBitVector::u64{0});
          if(n & 63) rank.bits[words] = (StaticBitVector::u64{1} << (n & 63))-1;
          rank.init();
          return 0;
        }
      }
    }
    for(int start = 0; start < n; start += 64) {
      StaticBitVector::u64 word = 0;
      int count = std::min(n-start,64);
      for(int j = 0; j < count; ++j) {
        word |= StaticBitVector::u64(bool((keys[start+j] >> bit) & 1)) << j;
      }
      rank.bits[start >> 6] = word;
    }
    rank.init();
    int zeros = rank.rank0(n);
    if(zeros == 0 || zeros == n) return zeros;

    buffer.clear();
    buffer.reserve(n);
    for(int one = 0; one < 2; ++one) {
      for(int start = 0; start < n; start += 64) {
        auto candidates = rank.bits[start >> 6];
        if(!one) candidates = ~candidates & (~StaticBitVector::u64{0} >> (64-std::min(n-start,64)));
        while(candidates) {
          int i = start+soryuusi::countr_zero(candidates);
          buffer.push_back(keys[i]);
          take(i);
          candidates &= candidates-1;
        }
      }
    }
    keys.swap(buffer);
    return zeros;
  }
};

}  // namespace soryuusi::internal
