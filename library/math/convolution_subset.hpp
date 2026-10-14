// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_subset.hpp
#pragma once
#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <vector>

#include "library/math/modint_traits.hpp"

namespace soryuusi {

using namespace std;

template<class T>
vector<T> convolution_subset(const vector<T>& a, const vector<T>& b){
  int n = a.size();
  int m = b.size();
  if(n == 0 || m == 0) return {};
  const int k = 31 - __builtin_clz((max(n,m) << 1) - 1);
  const int K = 1 << k;
  vector<T> c(K, 0);
  
  if(k <= 6){
    if constexpr (internal::is_modint_like_v<T>) {
      uint64_t mod = T::mod();
      for(int i = 0; i < K; ++i) for(int j = i; j < K; (++j)|=i) c[j] = T::raw((c[j].val() + (uint64_t)a[i].val() * b[j&~i].val()) % mod);
      return c;
    } else {
      for(int i = 0; i < K; ++i) for(int j = i; j < K; (++j)|=i) c[j] += a[i] * b[j&~i];
      return c;
    }
  }
  
  const int step = k + 1;
  vector<T> A(K * step, 0), B(K * step, 0), C(K * step, 0);
  for(int i = 0; i < n; ++i) A[i * step + __builtin_popcount(i)] = a[i];
  for(int i = 0; i < m; ++i) B[i * step + __builtin_popcount(i)] = b[i];
  for(int i = K>>1; i; i>>=1) for(int j = i; j < K; (++j)|=i) {
    T* pA_j = A.data() + j * step;
    T* pA_j_i = A.data() + (j & ~i) * step;
    T* pB_j = B.data() + j * step;
    T* pB_j_i = B.data() + (j & ~i) * step;
    for(int l = 0; l <= k; ++l) pA_j[l] += pA_j_i[l], pB_j[l] += pB_j_i[l];
  }

  if constexpr (internal::is_modint_like_v<T>) {
    uint64_t mod = T::mod();
    for(int i = 0; i < K; ++i) {
      T* pA = A.data() + i * step;
      T* pB = B.data() + i * step;
      T* pC = C.data() + i * step;
      for(int l = 0; l <= k; ++l) {
        unsigned __int128 sum = 0; 
        for(int l0 = 0; l0 <= l; ++l0) sum += (uint64_t)pA[l0].val() * pB[l - l0].val();
        pC[l] = T::raw(sum % mod);
      }
    }
  }else {
    for(int i = 0; i < K; ++i) {
      T* pA = A.data() + i * step;
      T* pB = B.data() + i * step;
      T* pC = C.data() + i * step;
      for(int l = 0; l <= k; ++l) for(int l0 = 0; l0 <= l; ++l0) pC[l] += pA[l0] * pB[l - l0];
    }
  }
  for(int i = K>>1; i; i>>=1) for(int j = i; j < K; (++j)|=i) {
    T* pC_j = C.data() + j * step;
    T* pC_j_i = C.data() + (j & ~i) * step;
    for(int l = 0; l <= k; ++l) pC_j[l] -= pC_j_i[l];
  }
  for(int i = 0; i < K; ++i) c[i] = C[i * step + __builtin_popcount(i)];
  return c;
}

}  // namespace soryuusi
