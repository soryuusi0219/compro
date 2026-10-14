// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_bit_xor_general.hpp
#pragma once
#include <algorithm>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
vector<T> convolution_bitxor_general(vector<T> a, vector<T> b){
  int n = a.size();
  int m = b.size();
  if(n == 0 || m == 0) return {};
  const int k = 31 - __builtin_clz((max(n,m) << 1) - 1);
  const int K = 1 << k;
  vector<T> c(K, 0);
  
  if(k <= 6){
    for(int i = 0; i < K; ++i) for(int j = 0; j < K; ++j) c[i^j] += a[i] * b[j];
    return c;
  }
  
  for(int i = K>>1; i; i>>=1) for(int j = i; j < K; (++j)|=i) a[j&~i] -= a[j], b[j&~i] -= b[j];
  const int step = k + 1;
  const int stepC = 2*k + 1;
  vector<T> A(K * step, 0), B(K * step, 0), C(K * stepC, 0);
  for(int i = 0; i < K; ++i) A[i * step + __builtin_popcount(i)] = a[i], B[i * step + __builtin_popcount(i)] = b[i];
  for(int i = K>>1; i; i>>=1) for(int j = i; j < K; (++j)|=i) {
    T* pA_j = A.data() + j * step;
    T* pA_j_i = A.data() + (j & ~i) * step;
    T* pB_j = B.data() + j * step;
    T* pB_j_i = B.data() + (j & ~i) * step;
    for(int l = 0; l <= k; ++l) pA_j[l] += pA_j_i[l], pB_j[l] += pB_j_i[l];
  }

  for(int i = 0; i < K; ++i) {
    T* pA = A.data() + i * step;
    T* pB = B.data() + i * step;
    T* pC = C.data() + i * stepC;
    for(int l1 = 0; l1 <= k; ++l1) for(int l2 = 0; l2 <= k; ++l2) pC[l1 + l2] += pA[l1] * pB[l2];
  }


  for(int i = K>>1; i; i>>=1) for(int j = i; j < K; (++j)|=i) {
    T* pC_j = C.data() + j * stepC;
    T* pC_j_i = C.data() + (j & ~i) * stepC;
    for(int l = 0; l < stepC; ++l) pC_j[l] -= pC_j_i[l];
  }

  T tmp = 1;
  for(int i = 0; i < K; ++i) {
    int pop = __builtin_popcount(i);
    tmp = 1;
    for(int j = pop; j < stepC; ++j, tmp += tmp) c[i] += C[i * stepC + j] * tmp;
  }
  for(int i = K>>1; i; i>>=1) for(int j = i; j < K; (++j)|=i) c[j&~i] += c[j];
  return c;
}

}  // namespace soryuusi
