// https://github.com/soryuusi0219/compro/blob/main/library/math/prime_table.hpp
#pragma once

#include <array>
#include <cmath>
#include <utility>
#include <vector>

namespace soryuusi {

inline std::vector<int>& PrimeTable(int n) {
  ++n;
  constexpr int block_size = 32768;
  static int done = 2;
  static int left = 1;
  static bool initialized = false;
  static std::vector<int> primes = {2};
  static std::vector<bool> sieve(block_size + 1, false);
  static std::vector<std::pair<int, int>> composites;

  if (!initialized) {
    initialized = true;
    for (int i = 3; i <= block_size; i += 2) {
      if (!sieve[i]) {
        composites.emplace_back(i, i * i / 2);
        for (int j = i * i; j <= block_size; j += 2 * i) sieve[j] = true;
      }
    }
  }
  if (done >= n) return primes;

  const int right = static_cast<long long>(block_size) *
                        (n + block_size - 1) / block_size +
                    1;
  done = right * 2;
  primes.reserve(static_cast<int>(done / std::log(done) * 1.1));
  for (; left < right; left += block_size) {
    std::array<bool, block_size> block{};
    for (auto& [prime, index] : composites) {
      for (; index < block_size + left; index += prime) block[index - left] = true;
    }
    for (int i = 0; i < block_size; ++i) {
      if (!block[i]) primes.emplace_back((left + i) * 2 + 1);
    }
  }
  return primes;
}

}  // namespace soryuusi
