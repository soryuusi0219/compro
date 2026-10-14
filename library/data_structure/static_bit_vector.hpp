// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/static_bit_vector.hpp
#pragma once

#include <cstdint>
#include <vector>

namespace soryuusi {

struct StaticBitVector {
  using u64 = std::uint64_t;

  std::vector<u64> bits;
  std::vector<int> prefix;

  StaticBitVector() = default;
  explicit StaticBitVector(int n) { build(n); }

  void build(int n) {
    bits.assign((n >> 6) + 2, 0);
    prefix.assign((n >> 6) + 2, 0);
  }

  void set(int i) { bits[i >> 6] |= 1ULL << (i & 63); }

  void init() {
    for (int i = 0; i + 1 < static_cast<int>(bits.size()); ++i) {
      prefix[i + 1] = prefix[i] + __builtin_popcountll(bits[i]);
    }
  }

  bool get(int i) const { return (bits[i >> 6] >> (i & 63)) & 1; }

  int rank1(int i) const {
    return prefix[i >> 6] +
           __builtin_popcountll(bits[i >> 6] & ~(~0ULL << (i & 63)));
  }

  int rank0(int i) const { return i - rank1(i); }
};

}  // namespace soryuusi
