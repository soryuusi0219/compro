// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_gcd_lcm.hpp
#pragma once
#include <array>
#include <cmath>
#include <utility>
#include <vector>

#include "library/math/prime_table.hpp"

namespace soryuusi {

using namespace std;



template<class T>
vector<T> convolution_gcd(vector<T> a,vector<T> b){
  if(!a.size() || !b.size()) return {};
  int n = a.size() - 1;
  auto pt = PrimeTable(n);
  for(auto&& p : pt) {
    if(p > n) break;
    for(int i = n/p; i; --i) a[i] += a[i*p], b[i] += b[i*p];
  }
  for(int i = 1; i <= n; ++i) a[i] *= b[i];
  for(auto&& p : pt) {
    if(p > n) break;
    for(int i = 1; i*p <= n; ++i) a[i] -= a[i*p];
  }
  return a;
}

template<class T>
vector<T> convolution_lcm(vector<T> a,vector<T> b){
  if(!a.size() || !b.size()) return {};
  int n = a.size() - 1;
  auto pt = PrimeTable(n);
  for(auto&& p : pt) {
    if(p > n) break;
    for(int i = 1; i*p <= n; ++i) a[i*p] += a[i], b[i*p] += b[i];
  }
  for(int i = 1; i <= n; ++i) a[i] *= b[i];
  for(auto&& p : pt) {
    if(p > n) break;
    for(int i = n/p; i; --i) a[i*p] -= a[i];
  }
  return a;
}

}  // namespace soryuusi
