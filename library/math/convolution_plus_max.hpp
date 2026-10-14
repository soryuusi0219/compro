// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_plus_max.hpp
#pragma once
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

#include "library/math/convolution.hpp"

namespace soryuusi {

using namespace std;

template<class T>
vector<T> plus_max_convolution(vector<T> a, vector<T> b){
  static constexpr unsigned long long MOD1 = 998244353;
  static constexpr unsigned long long MOD2 = 754974721;
  static constexpr long long M1M2 = MOD1 * MOD2;
  static constexpr unsigned long long inv1 = internal::inv_gcd(MOD2, MOD1).second;
  static constexpr unsigned long long inv2 = internal::inv_gcd(MOD1, MOD2).second;
  int n = a.size(), m = b.size();
  if(!n || !m) return {};
  int k = n + m;
  int B = 0; for(int i = 1; i < k; i <<= 1) B++;
  B = min(max((int)sqrt(B*k)*3,1),k);
  vector<pair<T, int>> x(k);
  for(int i = 0; i < n; i++) x[i] = {a[i],i};
  for(int i = 0; i < m; i++) x[n+i] = {b[i],n+i};
  sort(x.begin(), x.end());
  vector<T> res(k-1,0);
  int previ = 0;
  vector<T> a2(n,0), b2(m,0);
  for(int i = 0; i < k; i++){
    auto [val, id] = x[i];
    for(int j = previ; j < i; j++) if((x[j].second < n) != (id < n)) res[id + x[j].second - n] += val;
    if(id < n) a[id] = 0, a2[id] = 1;
    else b[id-n] = 0, b2[id-n] = 1; 
    if(i && i % B == 0){
      auto c1 = convolution<MOD1>(a,b2);
      auto d1 = convolution<MOD1>(b,a2);
      auto c2 = convolution<MOD2>(a,b2);
      auto d2 = convolution<MOD2>(b,a2);
      for(int j = 0; j < n + m - 1; j++){
        T t = ((c1[j]+d1[j]) * inv1) % MOD1 * MOD2 + ((c2[j]+d2[j]) * inv2) % MOD2 * MOD1;
        while(t+t > M1M2) t -= M1M2;
        res[j] += t;
      }
      for(;previ <= i; previ++){
        if(x[previ].second < n) a2[x[previ].second] = 0;
        else b2[x[previ].second-n] = 0;
      }
    }
  }
  return res;
}

}  // namespace soryuusi
