// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_bit_and.hpp
#pragma once
#include <algorithm>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
vector<T> convolution_bitand(vector<T> a,vector<T> b){
  int n = a.size();
  int m = b.size();
  if(n == 0 || m == 0) return {};
  int N = 31-__builtin_clz((max(n,m)<<1)-1);
  int K = 1 << N;
  a.resize(K), b.resize(K);
  for(int i = K >> 1; i; i>>=1) for(int j = i; j < K; (++j)|=i) a[j&~i] += a[j], b[j&~i] += b[j];
  for(int i = 0; i < K; i++) a[i] *= b[i];
  for(int i = K >> 1; i; i>>=1) for(int j = i; j < K; (++j)|=i) a[j&~i] -= a[j];
  return a;
}

}  // namespace soryuusi
