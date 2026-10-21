#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "library/string/rolling_hash.hpp"

namespace {

template<class Sequence>
int naive_lcp(const Sequence& a, int i, const Sequence& b, int j) {
  int length = 0;
  while (i + length < static_cast<int>(a.size()) &&
         j + length < static_cast<int>(b.size()) &&
         a[i + length] == b[j + length]) {
    ++length;
  }
  return length;
}

template<int HashCount, class Sequence>
void verify_sequence(const Sequence& a, const Sequence& b) {
  using Hash = soryuusi::RollingHash<HashCount, 42>;
  const Hash hash_a(a), hash_b(b);

  Hash pushed;
  for (const auto& value : a) pushed.push_back(value);
  assert(pushed.size() == static_cast<int>(a.size()));

  for (int left = 0; left <= static_cast<int>(a.size()); ++left) {
    for (int right = left; right <= static_cast<int>(a.size()); ++right) {
      assert(hash_a.get_hash(left, right) == pushed.get_hash(left, right));
    }
  }

  for (int l1 = 0; l1 <= static_cast<int>(a.size()); ++l1) {
    for (int l2 = 0; l2 <= static_cast<int>(a.size()); ++l2) {
      assert(hash_a.lcp(l1, l2) == naive_lcp(a, l1, a, l2));
      for (int length = 0;
           l1 + length <= static_cast<int>(a.size()) &&
           l2 + length <= static_cast<int>(a.size());
           ++length) {
        const bool expected = std::equal(a.begin() + l1, a.begin() + l1 + length,
                                         a.begin() + l2);
        assert(hash_a.is_same(l1, l1 + length, l2, l2 + length) == expected);
      }
    }
  }

  for (int i = 0; i <= static_cast<int>(a.size()); ++i) {
    for (int j = 0; j <= static_cast<int>(b.size()); ++j) {
      assert(hash_a.lcp(i, j, hash_b) == naive_lcp(a, i, b, j));
    }
  }
  assert(!hash_a.is_same(0, 0, 0, std::min<int>(1, b.size()), hash_b) ||
         b.empty());
}

}  // namespace

int main() {
  using Mint = soryuusi::RolHashMint;
  static_assert(Mint(Mint::MOD).val() == 0);
  static_assert(Mint(-1).val() == Mint::MOD - 1);
  static_assert((Mint(2) * Mint(3)).val() == 6);

  std::mt19937_64 rng(20261020);
  for (int trial = 0; trial < 100000; ++trial) {
    const uint64_t a = rng() % Mint::MOD;
    const uint64_t b = rng() % Mint::MOD;
    const auto product = static_cast<uint64_t>(
        static_cast<__uint128_t>(a) * b % Mint::MOD);
    assert((Mint(a) * Mint(b)).val() == product);
    assert((Mint(a) + Mint(b)).val() ==
           static_cast<uint64_t>((static_cast<__uint128_t>(a) + b) % Mint::MOD));
    assert((Mint(a) - Mint(b)).val() ==
           static_cast<uint64_t>((static_cast<__uint128_t>(a) + Mint::MOD - b) %
                                 Mint::MOD));
    const uint64_t c = rng() % Mint::MOD;
    assert(Mint::muladd(Mint(a), Mint(b), Mint(c)).val() ==
           (product + c) % Mint::MOD);
    assert(Mint::mulsub(Mint(a), Mint(b), Mint(c)).val() ==
           (product + Mint::MOD - c) % Mint::MOD);
    assert(Mint::submul(Mint(a), Mint(b), Mint(c)).val() ==
           (a + Mint::MOD - static_cast<uint64_t>(
                                static_cast<__uint128_t>(b) * c % Mint::MOD)) %
               Mint::MOD);
  }

  const std::vector<std::string> strings = {
      "", "a", "aaaa", "abacaba", "mississippi", "abcabcabxabc"};
  for (const auto& a : strings) {
    for (const auto& b : strings) {
      verify_sequence<1>(a, b);
      verify_sequence<3>(a, b);
    }
  }

  std::vector<int> a(40), b(35);
  for (int& value : a) value = static_cast<int>(rng() % 11) - 5;
  for (int& value : b) value = static_cast<int>(rng() % 11) - 5;
  verify_sequence<1>(a, b);
  verify_sequence<5>(a, b);

  const soryuusi::RolHashValue<3> x(7), y(11);
  assert(x + y == soryuusi::RolHashValue<3>(18));
  assert(x * y == soryuusi::RolHashValue<3>(77));
  assert(x < y && y > x && x <= x && y >= x);

  const soryuusi::RollingHash<2, 7> compatibility_alias(a);
  assert(compatibility_alias.size() == static_cast<int>(a.size()));

  std::cout << "rolling_hash: all tests passed\n";
}
