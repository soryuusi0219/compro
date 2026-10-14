// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_semi_online.hpp
#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <utility>
#include <vector>

#include "library/math/convolution_mod.hpp"
#include "library/math/modint_traits.hpp"
#include "library/math/ntt.hpp"

namespace soryuusi {

using namespace std;

template <typename T>
struct semi_online_convolution {
  using mint = T;
  using vm = vector<T>;

  int cnt = 0;
  vm a, b, c;
  int limit = 0;
  vector<vm> b_fixed_ntt;

  semi_online_convolution() = default;
  explicit semi_online_convolution(vm b0) { init(std::move(b0)); }

  void init(vm b0) {
    cnt = 0;
    b = std::move(b0);
    limit = static_cast<int>(b.size());
    a.assign(limit, T(0));
    c.assign(limit, T(0));
    b_fixed_ntt.clear();
  }

 private:
  static constexpr int ntt_threshold = 32;

  static int level(int len) {
    return __builtin_ctz(static_cast<unsigned int>(len));
  }

  static constexpr bool ntt_size_supported(int len) {
    if constexpr (!is_ntt_friendly_v<T>) {
      return false;
    } else {
      return std::int64_t{2} * len <=
             (std::int64_t{1} << NTTEngine<T>::rank2);
    }
  }

  const vm& fixed_ntt(int len) {
    const int k = level(len);
    if (static_cast<int>(b_fixed_ntt.size()) <= k) b_fixed_ntt.resize(k + 1);
    vm& transformed = b_fixed_ntt[k];
    if (transformed.empty()) {
      transformed.resize(2 * len);
      copy_n(b.begin() + len, len, transformed.begin());
      NTTEngine<T>::ntt(transformed);
    }
    return transformed;
  }

  void calculate_ntt(int la, int len) {
    vm result(2 * len);
    copy_n(a.begin() + la, len, result.begin());
    NTTEngine<T>::ntt(result);
    const vm& transformed_b = fixed_ntt(len);
    for (int i = 0; i < 2 * len; ++i) result[i] *= transformed_b[i];
    NTTEngine<T>::intt(result);
    const int offset = la + len;
    const int count = min(2 * len - 1, limit - offset);
    for (int i = 0; i < count; ++i) c[offset + i] += result[i];
  }

  void calculate_naive(int la, int lb, int len) {
    const int offset = la + lb;
    for (int i = 0; i < len; ++i) {
      for (int j = 0; j < len && i + j < limit - offset; ++j) {
        c[offset + i + j] += a[la + i] * b[lb + j];
      }
    }
  }

  void calculate_fallback(int la, int lb, int len) {
    if constexpr (internal::is_modint_like_v<T>) {
      if (len > 60 &&
          static_cast<std::uint64_t>(T::mod()) <= 2147483647ULL) {
        vm left(a.begin() + la, a.begin() + la + len);
        vm right(b.begin() + lb, b.begin() + lb + len);
        vm result = convolution_mod(left, right);
        const int offset = la + lb;
        const int count = min(static_cast<int>(result.size()), limit - offset);
        for (int i = 0; i < count; ++i) c[offset + i] += result[i];
        return;
      }
    }
    calculate_naive(la, lb, len);
  }

 public:
  void _cul(int la, int lb, int len) {
    const int offset = la + lb;
    if (offset >= limit) return;
    const int clipped_len = min(len, limit - offset);
    if constexpr (is_ntt_friendly_v<T>) {
      if (lb == len && clipped_len == len && len >= ntt_threshold &&
          ntt_size_supported(len)) {
        calculate_ntt(la, len);
        return;
      }
    }
    calculate_fallback(la, lb, clipped_len);
  }

  mint push(mint a0) {
    assert(cnt < limit);
    a[cnt] = a0;
    int n = ++cnt;
    _cul(n - 1, 0, 1);
    _cul(n - 1, 1, 1);
    if (n > 1 && (n & 1)) {
      n--;
      int m = n & -n;
      int s = 2;
      for (int i = 2; i <= m; i <<= 1) {
        _cul(n - i, s, i);
        s += i;
      }
    }
    return c[cnt - 1];
  }
};

}  // namespace soryuusi
