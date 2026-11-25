// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/hash_table_storage.hpp
#pragma once
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "../../math/internal/internal_bit.hpp"

namespace soryuusi::internal {

template<class K, class V, auto hash, bool DoubleHash = true>
class HashTableStorage {
  static std::size_t capacity(int n) {
    if(n < 0 || static_cast<std::size_t>(n) > (static_cast<std::size_t>(-1) >> 2)) throw std::length_error("invalid hash table capacity");
    return soryuusi::bit_ceil(2 * static_cast<std::size_t>(n));
  }

  std::size_t mask;
  std::vector<unsigned char> tags;
  std::vector<K> keys;
  std::vector<V> values;

  template<bool Insert>
  std::size_t find(K k, auto initial) {
    std::uint64_t h = hash(k);
    std::size_t start = h & mask;
    // An odd stride visits every slot in a power-of-two table.
    std::size_t step = DoubleHash ? ((h >> 32) | 1) : 1;
    unsigned char tag = static_cast<unsigned char>((h >> 57) + 1);
    std::size_t i = start;
    while(tags[i]) {
      if(tags[i] == tag && !(keys[i] != k)) return i;
      i = (i+step) & mask;
      if(i == start) return tags.size();
    }
    if constexpr(Insert) {
      keys[i] = k;
      values[i] = initial();
      tags[i] = tag;
      return i;
    }
    return tags.size();
  }

public:
  explicit HashTableStorage(int n) : mask(capacity(n)-1), tags(mask+1), keys(mask+1), values(mask+1) {}

  std::size_t lookup(const K& k) {
    return find<false>(k,nullptr);
  }

  std::size_t insert(const K& k, auto initial) {
    std::size_t i = find<true>(k,initial);
    if(i == tags.size()) throw std::length_error("hash table is full");
    return i;
  }

  std::size_t end() const { return tags.size(); }
  decltype(auto) key(std::size_t i) const { return keys[i]; }
  decltype(auto) value(std::size_t i) { return values[i]; }
};

}  // namespace soryuusi::internal
