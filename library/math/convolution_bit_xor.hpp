// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_bit_xor.hpp
#pragma once
#include <algorithm>
#include <cmath>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
vector<T> convolution_bitxor(vector<T> a,vector<T> b){
  int n = a.size();
  int m = b.size();
  if(n == 0 || m == 0) return {};
  int N = 31-__builtin_clz((max(n,m)<<1)-1);
  int K = 1 << N;
  a.resize(K), b.resize(K);
  for(int i = K>>1; i; i>>=1) for(int j = i; j < K; (++j)|=i) {
    a[j] = (a[j&~i] += a[j]) - a[j] - a[j];
    b[j] = (b[j&~i] += b[j]) - b[j] - b[j];
  }
  T t = T(2).pow(N).inv();
  for(int i = 0; i < (1 << N); i++) a[i] *= b[i] * t;
  for(int i = K>>1; i; i>>=1) for(int j = i; j < K; (++j)|=i) {
    a[j] = (a[j&~i] += a[j]) - a[j] - a[j];
  }
  return a;
}

}  // namespace soryuusi
