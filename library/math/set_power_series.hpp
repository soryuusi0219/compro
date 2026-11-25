// https://github.com/soryuusi0219/compro/blob/main/library/math/set_power_series.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <span>
#include <utility>
#include <vector>

#include "internal/internal_bit.hpp"
#include "convolution_subset.hpp"
#include "modint_factorials.hpp"

namespace soryuusi {

using namespace std;

template<typename T>
struct SetPowerSeries : vector<T>{
  using vector<T>::vector;
  using SPS = SetPowerSeries;
  template<class U> SetPowerSeries(const vector<U>& v){
    const auto size = soryuusi::bit_ceil(v.size());
    this->reserve(size);
    this->insert(this->end(),v.begin(),v.end());
    this->resize(size);
  }
  SetPowerSeries(vector<T>&& v) : vector<T>(std::move(v)){
    this->resize(soryuusi::bit_ceil(this->size()));
  }
  int dim() const {return this->empty() ? -1 : soryuusi::countr_zero(this->size());}

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
  friend SPS operator+(SPS x, const SPS& y) {x += y; return x;}
  friend SPS operator-(SPS x, const SPS& y) {x -= y; return x;}
  friend SPS operator*(SPS x, const T y) {x *= y; return x;}
  friend SPS operator/(SPS x, const T y) {x /= y; return x;}
  SPS operator-() const {
    SPS res(*this);
    for(auto& x : res) x = -x;
    return res;
  }
  friend SPS operator*(const SPS& x, const SPS& y) {return convolution_subset(x,y);}
  friend SPS& operator*=(SPS& x, const SPS& y) {return x = x * y;}
  friend SPS operator/(const SPS& x, const SPS& y) {
    if(x.empty()) return SPS(1);
    assert(!y.empty());
    const auto n = soryuusi::bit_ceil(std::max(x.size(),y.size()));
    if(y.size() == 1){
      SPS result(x.begin(),x.end());
      result.resize(n);
      result /= y[0];
      return result;
    }
    auto block = [](const SPS& f, std::size_t k){
      const auto first = std::min(k,f.size());
      return std::span<const T>(f).subspan(first,std::min(k,f.size()-first));
    };
    // The upper half of x/y only needs the inverse of the lower half of y.
    SPS inverse(n/2), result(n);
    std::copy_n(y.begin(),std::min(n/2,y.size()),inverse.begin());
    inverse = inverse.inv();
    result[0] = x[0]*inverse[0];
    const std::span<T> out(result);
    for(std::size_t k = 1; k < n; k <<= 1){
      internal::SubsetConvolution<T> plan(k);
      auto high = out.subspan(k,k);
      const auto x_high = block(x,k), y_high = block(y,k);
      if(!y_high.empty()){
        plan.set_left(y_high);
        plan.multiply(out.first(k),high);
        for(T& value : high) value = -value;
      }
      for(std::size_t i = 0; i < x_high.size(); ++i) high[i] += x_high[i];
      plan.set_left(std::span<const T>(inverse).first(k));
      plan.multiply(high,high);
    }
    return result;
  }
  friend SPS& operator/=(SPS& x, const SPS& y) {return x = x/y;}

private:
  template<bool Log>
  SPS inverse_blocks() const {
    const auto n = this->size();
    SPS g{T(1)/this->front()}, result;
    g.reserve(Log ? std::max<std::size_t>(1,n/2) : n);
    if constexpr (Log) result.resize(n);
    vector<T> tail;
    for(std::size_t k = 1; k < n; k <<= 1){
      // Lifting by one element splits off a square-zero term.
      // log(f) gains f_high*g, and the inverse gains -(f_high*g)*g.
      internal::SubsetConvolution<T> plan(k);
      plan.set_left(g);
      tail.resize(k);
      plan.multiply(std::span<const T>(*this).subspan(k,k),tail);
      if constexpr (Log){
        std::copy(tail.begin(),tail.end(),result.begin()+k);
        if(2*k == n) break;
      }
      g.resize(2*k);
      auto high = std::span<T>(g).last(k);
      plan.multiply(tail,high);
      for(T& x : high) x = -x;
    }
    if constexpr (Log) return result;
    else return g;
  }

  SPS power_blocks(SPS g, T exponent) const {
    const auto n = this->size();
    g.reserve(n);
    for(std::size_t k = g.size(); k < n; k <<= 1){
      // The same lower-half power occurs in both parts of the next power.
      exponent += T(1);
      internal::SubsetConvolution<T> plan(k);
      plan.set_left(g);
      g.resize(2*k);
      const std::span<const T> f(*this);
      const std::span<T> out(g);
      if(exponent != T(0)){
        plan.multiply(f.subspan(k,k),out.last(k));
        for(T& x : out.last(k)) x *= exponent;
      }
      plan.multiply(f.first(k),out.first(k));
    }
    return g;
  }

public:
  SPS inv() const {
    assert(soryuusi::has_single_bit(this->size()));
    return inverse_blocks<false>();
  }
  SPS exp() const {
    assert(soryuusi::has_single_bit(this->size()) && this->front() == T(0));
    SPS g(this->size());
    g[0] = T(1);
    const std::span<const T> f(*this);
    const std::span<T> out(g);
    for(std::size_t k = 1; k < this->size(); k <<= 1){
      internal::SubsetConvolution<T> plan(k);
      plan.set_left(f.subspan(k,k));
      plan.multiply(out.first(k),out.subspan(k,k));
    }
    return g;
  }
  SPS log() const {
    assert(soryuusi::has_single_bit(this->size()) && this->front() == T(1));
    return inverse_blocks<true>();
  }
  SPS pow(int m) const {
    const auto n = this->size();
    assert(soryuusi::has_single_bit(n));
    if(m == 0){
      SPS identity(n);
      identity[0] = T(1);
      return identity;
    }
    if(m == 1) return *this;
    if(m == 2) return *this * *this;
    if(m == -1) return inv();
    const int bits = dim();
    if(0 < m && m < bits){
      SPS g(n >> m);
      g[0] = T(1);
      return power_blocks(std::move(g),T(0));
    }
    const auto exponent = static_cast<long long>(m)-bits;
    T base = this->front(), constant = 1;
    if(exponent < 0) base = T(1)/base;
    for(auto k = exponent < 0 ? -exponent : exponent; k; k >>= 1){
      if(k&1) constant *= base;
      if(k > 1) base *= base;
    }
    return power_blocks(SPS{constant},T(exponent));
  }
  SPS pow_rational(T m) const {
    assert(soryuusi::has_single_bit(this->size()) && this->front() == T(1));
    if(m == T(0)) return pow(0);
    if(m == T(1)) return *this;
    if(m == T(2)) return *this * *this;
    return power_blocks(SPS{T(1)},m-T(dim()));
  }
  SPS sqrt() const {return pow_rational(T(1)/T(2));}
};

template<class T, class Factorials = modint_factorials<T>>
vector<T> power_projection(const SetPowerSeries<T>& f, const SetPowerSeries<T>& g, int m){
  assert(m >= 0 && soryuusi::has_single_bit(f.size()) && f.size() == g.size());
  if(m == 0) return {};
  if(m == 1) return {g.back()};
  const int limit = std::min(f.dim(),m-1);
  vector<vector<T>> weights;
  weights.reserve(limit+1);
  weights.push_back(g);
  vector<T> product;
  int depth = 0;
  for(std::size_t k = f.size()/2; k; k >>= 1){
    internal::SubsetConvolution<T> plan(k);
    plan.set_left(std::span<const T>(f).subspan(k,k));
    product.resize(k);
    const int next = std::min(depth+1,limit);
    if(next > depth) weights.emplace_back(k);
    // The full-set coefficient pairs opposite halves under disjoint union.
    for(int i = next; i > 0; --i){
      plan.multiply(std::span<const T>(weights[i-1]).first(k),product);
      if(i <= depth){
        for(std::size_t s = 0; s < k; ++s) weights[i][s] = weights[i][k+s]+product[s];
        weights[i].resize(k);
      }else std::copy(product.begin(),product.end(),weights[i].begin());
    }
    auto& first = weights[0];
    std::copy_n(first.begin()+k,k,first.begin());
    first.resize(k);
    depth = next;
  }

  vector<T> result(m);
  const T c = f[0];
  if(c == T(0)){
    T fact = 1;
    for(int i = 0; i <= limit; ++i){
      result[i] = weights[i][0]*fact;
      fact *= T(i+1);
    }
  }else{
    vector<T> powers(m,T(1)), coefficient(limit+1);
    if(c != T(1)) for(int i = 1; i < m; ++i) powers[i] = powers[i-1]*c;
    for(int i = 0; i <= limit; ++i) coefficient[i] = weights[i][0];
    for(int j = 0; j < m; ++j){
      T falling = 1;
      for(int i = 0, d = std::min(j,limit); i <= d; ++i){
        result[j] += coefficient[i]*falling*powers[j-i];
        falling *= T(j-i);
      }
    }
  }
  return result;
}

template<class T>
SetPowerSeries<T> composition(const vector<T>& F, const SetPowerSeries<T>& f){
  using SPS = SetPowerSeries<T>;
  assert(soryuusi::has_single_bit(f.size()));
  if(F.size() <= 1){
    SPS result(f.size());
    if(!F.empty()) result[0] = F[0];
    return result;
  }
  if(F.size() == 2){
    SPS result = f*F[1];
    result[0] += F[0];
    return result;
  }
  const int limit = std::min<std::size_t>(f.dim(),F.size()-1);
  vector<SPS> g(limit+1,SPS(1));
  const T c = f[0];
  if(c == T(0)){
    T fact = 1;
    for(int i = 0; i <= limit; ++i){
      g[i][0] = F[i]*fact;
      fact *= T(i+1);
    }
  }else{
    vector<T> derivatives(limit+1);
    int active = 0;
    for(auto it = F.rbegin(); it != F.rend(); ++it){
      for(int i = active; i > 0; --i) derivatives[i] = derivatives[i]*c+derivatives[i-1]*T(i);
      derivatives[0] = derivatives[0]*c+*it;
      active = std::min(active+1,limit);
    }
    for(int i = 0; i <= limit; ++i) g[i][0] = derivatives[i];
  }

  for(std::size_t k = 1; k < f.size(); k <<= 1){
    internal::SubsetConvolution<T> plan(k);
    plan.set_left(std::span<const T>(f).subspan(k,k));
    const auto remaining = std::min<std::size_t>(g.size(),f.dim()-soryuusi::countr_zero(k));
    for(std::size_t i = 0; i < remaining; ++i){
      g[i].resize(2*k);
      // Lifting by one element applies the next ordinary derivative to the upper half.
      if(i+1 < g.size()) plan.multiply(g[i+1],std::span<T>(g[i]).last(k));
    }
    g.resize(remaining);
  }
  return std::move(g[0]);
}

}  // namespace soryuusi
