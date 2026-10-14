// https://github.com/soryuusi0219/compro/blob/main/library/math/set_power_series.hpp
#pragma once
#include <algorithm>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

#include "library/math/modint_traits.hpp"

#include "library/math/modint_factorials.hpp"

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

template<typename T>
struct SetPowerSeries : vector<T>{
  using vector<T>::vector;
  using SPS = SetPowerSeries;
  template<class U> SetPowerSeries(const vector<U>& v){
    this->assign(std::bit_ceil(v.size()),0);
    copy(v.begin(), v.end(), this->begin());
  }
  inline int dim() const {return this->size() ? __builtin_ctz(this->size()) : -1;}

  friend SPS& operator+=(SPS& x, const SPS& y){
    for(int i = 0; i < (int)(x.size()); ++i) x[i] += y[i];
    return x;
  }
  friend SPS& operator-=(SPS& x, const SPS& y){
    for(int i = 0; i < (int)(x.size()); ++i) x[i] -= y[i];
    return x;
  }
  friend SPS& operator*=(SPS& x, const T y){
    for(int i = 0; i < (int)(x.size()); ++i) x[i] *= y;
    return x;
  }
  friend SPS& operator/=(SPS& x, const T y) {return x *= (T(1)/y);}
  friend SPS operator+(SPS x, const SPS& y) {return x += y;}
  friend SPS operator-(SPS x, const SPS& y) {return x -= y;}
  friend SPS operator*(SPS x, const T y) {return x *= y;}
  friend SPS operator/(SPS x, const T y) {return x /= y;}
  SPS operator-() const {
    SPS res(*this);
    for(auto& x : res) x = -x;
    return res;
  }
  friend SPS operator*(const SPS& x, const SPS& y) {return convolution_subset(x, y);}
  friend SPS& operator*=(SPS& x, const SPS& y) {return x = x * y;}
  friend SPS operator/(const SPS& x, const SPS& y) {return x * y.inv();}
  friend SPS& operator/=(SPS& x, const SPS& y) {return x *= y.inv();}

  SPS inv() const {
    SPS g = {T(1)/this->front()};
    int k = 1;
    int n = this->size();
    while(k < n){
      SPS f0 = {this->begin()+k, this->begin()+(k << 1)};
      SPS h = -(f0*g*g);
      g.insert(g.end(),h.begin(),h.end());
      k <<= 1;
    }
    return g;
  }

  SPS exp() const {
    assert(this->front() == 0);
    SPS g = {1};
    for(int k = 1; k < (int)this->size(); k <<= 1){
      SPS f0 = {this->begin()+k, this->begin()+(k << 1)};
      SPS h = f0*g;
      g.insert(g.end(),h.begin(),h.end());
    }
    return g;
  }
  SPS log() const {
    assert(this->front() == 1);
    SPS g = inv();
    int n = this->size();
    SPS res(n,0);
    while(n >>= 1){
      SPS f0 = {this->begin()+n, this->begin()+(n<<1)};
      g.resize(n);
      SPS h = f0*g;
      copy(h.begin(),h.end(),res.begin()+n);
    }
    return res;
  }
  SPS pow(int m) const {
    int n = dim();
    SPS g = {1};
    int k = 1;
    m -= n;
    if(m < 0){
      k <<= (-m);
      g.resize(k);
      m = 0;
    }else g[0] = (*this)[0].pow(m);
    int N = this->size();
    while(k < N){
      SPS fl = {this->begin(),this->begin()+k}, fh = {this->begin()+k,this->begin()+(k<<1)};
      SPS h1 = fl*g;
      SPS h2 = fh*g*m;
      g.swap(h1);
      g.insert(g.end(),h2.begin(),h2.end());
      k <<= 1;
    }
    return g;
  }
  SPS pow_rational(T m) const {
    assert((*this)[0] == 1);
    SPS g = {1};
    int k = 1;
    int n = dim();
    int N = this->size();
    m -= n;
    while(k < N){
      m++;
      SPS fl = {this->begin(),this->begin()+k}, fh = {this->begin()+k,this->begin()+(k<<1)};
      SPS h1 = fl*g;
      SPS h2 = fh*g*m;
      g.swap(h1);
      g.insert(g.end(),h2.begin(),h2.end());
      k <<= 1;
    }
    return g;
  }
  SPS sqrt() const {return pow_rational(T(1)/T(2));}
};

template<class T, class Factorials = modint_factorials<T>>
vector<T> power_projection(const SetPowerSeries<T>& f, const SetPowerSeries<T>& g, int m){
  Factorials factorials;
  int n = f.dim();
  int N = 1 << n;
  vector<SetPowerSeries<T>> G = {g};
  reverse(G[0].begin(), G[0].end());
  int k = N;
  while(k >>= 1){
    SetPowerSeries<T> f0(f.begin() + k, f.begin() + (k << 1));
    vector<SetPowerSeries<T>> NG = G;
    for(auto& g0 : NG) g0.resize(k);
    NG.emplace_back(k, 0);
    for(int i = 0; i < (int)G.size(); ++i){
      SetPowerSeries<T> g0(G[i].rbegin(), G[i].rbegin() + k);
      g0 *= f0;
      for(int j = 0; j < k; j++) NG[i+1][j] += g0[k-j-1];
    }
    G.swap(NG);
  }
  vector<T> res(m, 0);
  T c = f[0];
  T fact = 1;
  for(int i = 0; i < n+1; ++i){
    T tmp = G[i][0] * fact;
    for(int j = i; j < m; ++j){
      res[j] += tmp;
      tmp *= c * T(j+1) * factorials.mint_inv(j-i+1);
    }
    fact *= T(i+1);
  }
  return res;
}

template<class T>
SetPowerSeries<T> composition(const vector<T>& F, const SetPowerSeries<T>& f){
  int n = f.dim(), m = F.size(), N = 1 << n;
  vector<SetPowerSeries<T>> g(n+1, SetPowerSeries<T>(1,0));
  if(f[0] == 0) {
    T c = 1;
    for(int i = 0; i < min(n+1, m); ++i) g[i][0] = F[i] * c, c *= i+1;
  }
  else{
    T ai = T(1)/f[0], ap = 1;
    for(int i = 0; i < m; ++i) {
      T c = F[i] * ap;
      for(int j = 0; j <= min(i+1,n); ++j) g[j][0] += c, c *= ai * (i-j);
      ap *= f[0];
    }
  }
  int k = 1;
  while(k < N){
    SetPowerSeries<T> f0(f.begin() + k, f.begin() + (k << 1));
    vector<SetPowerSeries<T>> ng = g;
    ng.pop_back();
    for(int i = 0; i < (int)ng.size(); ++i){
      g[i+1] *= f0;
      ng[i].insert(ng[i].end(), g[i+1].begin(), g[i+1].end());
    }
    g.swap(ng);
    k <<= 1;
  }
  return g[0];
}

}  // namespace soryuusi
