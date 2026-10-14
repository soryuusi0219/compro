// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_online.hpp
#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

#include "library/math/convolution_mod.hpp"
#include "library/math/modint_traits.hpp"
#include "library/math/ntt.hpp"

namespace soryuusi {

using namespace std;

template <typename T>
struct online_convolution {
  using mint = T;
  using vm = vector<T>;

  int cnt = 0;
  vm a, b, c;
  int limit = 0x10000000;
  bool lim_flag = false;
  vector<vm> a_fixed_ntt, b_fixed_ntt;

  online_convolution() = default;
  explicit online_convolution(int lim) : limit(lim), lim_flag(true) {
    a.resize(limit);
    b.resize(limit);
    c.resize(limit);
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

  static vm make_ntt(const vm& values, int first, int len) {
    vm result(2 * len);
    copy_n(values.begin() + first, len, result.begin());
    NTTEngine<T>::ntt(result);
    return result;
  }

  const vm& fixed_ntt(const vm& values, vector<vm>& cache, int len) {
    const int k = level(len);
    if (static_cast<int>(cache.size()) <= k) cache.resize(k + 1);
    if (cache[k].empty()) cache[k] = make_ntt(values, len - 1, len);
    return cache[k];
  }

  void calculate_ntt(int la, int lb, int len) {
    vm result;
    vm other_buffer;
    const vm* other;
    if (la != len - 1) {
      result = make_ntt(a, la, len);
      if (lb == len - 1) {
        other = &fixed_ntt(b, b_fixed_ntt, len);
      } else {
        other_buffer = make_ntt(b, lb, len);
        other = &other_buffer;
      }
    } else if (lb != len - 1) {
      result = make_ntt(b, lb, len);
      other = &fixed_ntt(a, a_fixed_ntt, len);
    } else {
      result = fixed_ntt(a, a_fixed_ntt, len);
      other = &fixed_ntt(b, b_fixed_ntt, len);
    }

    for (int i = 0; i < 2 * len; ++i) result[i] *= (*other)[i];
    NTTEngine<T>::intt(result);
    const int offset = la + lb;
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
      if (clipped_len == len && len >= ntt_threshold &&
          ntt_size_supported(len)) {
        calculate_ntt(la, lb, len);
        return;
      }
    }
    calculate_fallback(la, lb, clipped_len);
  }

  mint push(mint a0, mint b0) {
    if (!lim_flag) {
      assert(cnt < limit);
      a.push_back(a0);
      b.push_back(b0);
      c.resize(min(limit, 2 * cnt + 1), T(0));
    } else {
      assert(cnt < limit);
      a[cnt] = a0;
      b[cnt] = b0;
    }

    const int n = ++cnt;
    int m = (n + 1) & (-n - 1);
    int s = 0;
    if (m <= n) {
      for (int i = 1; i <= m; i <<= 1) {
        _cul(n - i, s, i);
        _cul(s, n - i, i);
        s += i;
      }
    } else {
      m >>= 1;
      for (int i = 1; i < m; i <<= 1) {
        _cul(n - i, s, i);
        _cul(s, n - i, i);
        s += i;
      }
      _cul(s, n - m, m);
    }
    return c[n - 1];
  }
};

}  // namespace soryuusi
