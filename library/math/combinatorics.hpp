// https://github.com/soryuusi0219/compro/blob/main/library/math/combinatorics.hpp
#pragma once
#include <algorithm>
#include <bit>
#include <cassert>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

#include "formal_power_series.hpp"
#include "internal_fps_composition.hpp"
#include "internal_math.hpp"
#include "linear_sieve.hpp"
#include "modint_factorials.hpp"
#include "modint_traits.hpp"

namespace soryuusi {

namespace internal {

template<class T>
std::vector<T> integer_power_table(int n, long long exponent){
  assert(n >= 0 && exponent >= 0);
  if(exponent == 0) return std::vector<T>(n,T(1));
  std::vector<T> powers(n);
  if(n <= 1) return powers;
  const Linear_Sieve sieve(n);
  powers[1] = 1;
  for(int i = 2; i < n; ++i){
    const int p = sieve[i];
    powers[i] = p == i ? T(i).pow(exponent) : powers[p]*powers[i/p];
  }
  return powers;
}

// Extend samples at 0,...,d to consecutive integer points using Lagrange weights.
template<class T>
void extend_factorial_samples(std::vector<T>& values, int count, modint_factorials<T>& factorials){
  const int d = values.size()-1;
  if(count <= d+1) return;
  std::vector<T> weights(d+1), inverse(count-1);
  for(int i = 0; i <= d; ++i){
    weights[i] = values[i]*factorials.ifact(i)*factorials.ifact(d-i);
    if((d-i)&1) weights[i] = -weights[i];
  }
  for(int i = 0; i < count-1; ++i) inverse[i] = factorials.inv(i+1);
  auto extension = fps_middle_product<T>(std::move(inverse),std::move(weights));
  values.resize(count);
  for(int i = d+1; i < count; ++i) values[i] = extension[i-d-1]*factorials.fact(i)*factorials.ifact(i-d-1);
}

// prefix[j] = (j*block)!.
template<class T>
std::vector<T> factorial_blocks(int block, int count){
  modint_factorials<T> factorials(std::max(2*block+1,count-1));
  std::vector<T> values{T(1),T(2)};
  for(int d = 1; d < block; d *= 2){
    extend_factorial_samples(values,4*d+2,factorials);
    // The degree-d block at 2*d*i and the next one form the degree-2d block.
    for(int i = 0; i <= 2*d; ++i) values[i] = values[2*i]*values[2*i+1];
    values.resize(2*d+1);
  }
  extend_factorial_samples(values,count,factorials);
  values.resize(count+1);
  for(int i = count; i > 0; --i) values[i] = values[i-1];
  values[0] = 1;
  for(int i = 1; i <= count; ++i) values[i] *= values[i-1];
  return values;
}

template<class T>
T factorial_tail(T product, std::uint64_t first, std::uint64_t last){
  if(last-first >= 8){
    T b = 1, c = 1, d = 1;
    for(; last-first >= 4; first += 4){
      product *= T::raw(first+1);
      b *= T::raw(first+2);
      c *= T::raw(first+3);
      d *= T::raw(first+4);
    }
    product *= b*c*d;
  }
  for(; first < last; ++first) product *= T::raw(first+1);
  return product;
}

template<class T>
FormalPowerSeries<T> stirling_first_row(int n){
  using FPS = FormalPowerSeries<T>;
  if(n == 0) return FPS{T(1)};
  const int half = n/2;
  FPS result = stirling_first_row<T>(half);
  result *= result.Taylor_shift(T(-half));
  if(n&1){
    result.push_back(0);
    for(int i = n; i > 0; --i) result[i] = result[i-1]-T(n-1)*result[i];
    result[0] *= T(1-n);
  }
  return result;
}

template<bool First, class T>
std::vector<T> stirling_column(int n, int k){
  assert(0 <= k && k <= n);
  const int count = n-k+1;
  if(k == 0){
    std::vector<T> result(count);
    result[0] = 1;
    return result;
  }
  if(count == 1) return {T(1)};
  if(k == 1){
    std::vector<T> result(count,T(1));
    if constexpr (First)
      for(int i = 1; i < count; ++i) result[i] = -result[i-1]*T(i);
    return result;
  }
  using FPS = FormalPowerSeries<T>;
  auto& factorials = FPS::factorials;
  factorials.precompute(count);
  FPS base(count);
  for(int i = 0; i < count; ++i){
    if constexpr (First) base[i] = (i&1 ? -factorials.inv(i+1) : factorials.inv(i+1));
    else base[i] = factorials.ifact(i+1);
  }
  FPS result = k == 2 ? base*base : base.pow(k);
  result.resize(count);
  T scale = 1;
  for(int i = 1; i < count; ++i){
    scale *= T(k+i);
    result[i] *= scale;
  }
  return result;
}

}  // namespace internal

template<class T, std::integral Index>
std::vector<T> many_factorials(const std::vector<Index>& queries){
  const std::uint64_t mod = T::mod();
  std::vector<T> result(queries.size());
  if(queries.empty() || mod == 1) return result;
  const bool prime = [&]{
    if constexpr (internal::is_static_modint_like_v<T>) return internal::is_prime64<T::mod()>;
    else return internal::is_prime64_runtime(mod);
  }();
  std::vector<std::pair<std::uint64_t,std::size_t>> order;
  std::vector<std::size_t> reflected;
  order.reserve(queries.size());
  for(std::size_t i = 0; i < queries.size(); ++i){
    if constexpr (std::signed_integral<Index>) assert(queries[i] >= 0);
    const auto n = static_cast<std::uint64_t>(queries[i]);
    if(n >= mod) continue;
    const auto mirror = mod-1-n;
    if(prime && mirror < n){
      order.emplace_back(mirror,i);
      reflected.push_back(i);
    }else order.emplace_back(n,i);
  }
  if(order.empty()) return result;
  std::sort(order.begin(),order.end());

  const auto max_n = order.back().first;
  std::uint64_t block = 0;
  std::vector<T> prefix;
  if(prime && max_n > (1<<16) && max_n/32 > order.size() && mod <= std::numeric_limits<int>::max()){
    // Balance the number of block samples with the short products at queries.
    const auto target = std::sqrt(double(max_n)/(8.0+order.size()/56.0));
    block = std::bit_ceil(std::max<std::uint64_t>(1,static_cast<std::uint64_t>(target)));
    prefix = internal::factorial_blocks<T>(block,max_n/block);
  }
  std::uint64_t current = 0;
  T value = 1;
  for(auto [n,i] : order){
    if(block){
      const auto group = n/block;
      if(group*block > current){
        current = group*block;
        value = prefix[group];
      }
    }
    if(value != T(0)) value = internal::factorial_tail(value,current,n);
    current = n;
    result[i] = value;
  }

  if(!reflected.empty()){
    std::vector<T> products(reflected.size()+1,T(1));
    for(std::size_t i = 0; i < reflected.size(); ++i) products[i+1] = products[i]*result[reflected[i]];
    T inverse = products.back().inv();
    for(std::size_t i = reflected.size(); i > 0; --i){
      const auto index = reflected[i-1];
      const T reciprocal = inverse*products[i-1];
      inverse *= result[index];
      result[index] = (queries[index]&1) ? reciprocal : -reciprocal;
    }
  }
  return result;
}

template<class T>
T factorial(long long n){
  assert(n >= 0);
  if(static_cast<std::uint64_t>(n) >= static_cast<std::uint64_t>(T::mod())) return 0;
  if(n <= 1<<16) return internal::factorial_tail(T(1),0,n);
  return many_factorials<T>(std::vector<long long>{n})[0];
}

template<class T>
std::vector<T> bernoulli_numbers(int n){
  assert(n >= 0);
  std::vector<T> result(n+1);
  result[0] = 1;
  if(n == 0) return result;
  const T half = T(2).inv();
  result[1] = -half;
  if(n == 1) return result;
  const int count = n/2+1;
  const auto inverse = internal::factorial_and_inverse_factorials<T>(2*count-1).second;
  FormalPowerSeries<T> cosine(count), sine(count);
  for(int i = 0; i < count; ++i){
    cosine[i] = inverse[2*i];
    sine[i] = inverse[2*i+1];
  }
  // B(x)+x/2 = (x/2)*coth(x/2); retain only the even powers.
  cosine /= sine;
  const T quarter = half*half;
  T scale = 1;
  for(int i = 1; i < count; ++i){
    scale *= T(2*i)*T(2*i-1)*quarter;
    result[2*i] = cosine[i]*scale;
  }
  return result;
}

template<class T>
std::vector<T> bell_numbers(int n){
  assert(n >= 0);
  using FPS = FormalPowerSeries<T>;
  auto& factorials = FPS::factorials;
  factorials.precompute(n);
  FPS egf(n+1);
  for(int i = 1; i <= n; ++i) egf[i] = factorials.ifact(i);
  FPS result = egf.exp();
  for(int i = 0; i <= n; ++i) result[i] *= factorials.fact(i);
  return result;
}

inline std::vector<std::uint64_t> montmort_numbers(int n, std::uint64_t mod){
  assert(n >= 0 && mod > 0);
  std::vector<std::uint64_t> result(n);
  if(mod == 1) return result;
  auto fill = [&]<class UInt>(){
    std::uint64_t value = 1;
    for(int i = 1; i <= n; ++i){
      value = (UInt(value)*i+((i&1) ? mod-1 : 1))%mod;
      result[i-1] = value;
    }
  };
  if(mod <= std::numeric_limits<std::uint32_t>::max()) fill.template operator()<std::uint64_t>();
  else fill.template operator()<__uint128_t>();
  return result;
}

template<class T>
std::vector<T> partition_numbers(int n){
  assert(n >= 0);
  FormalPowerSeries<T> euler(n+1);
  euler[0] = 1;
  for(long long k = 1; k*(3*k-1)/2 <= n; ++k){
    const auto first = k*(3*k-1)/2, second = first+k;
    const T sign = k&1 ? T(-1) : T(1);
    euler[first] = sign;
    if(second <= n) euler[second] = sign;
  }
  return euler.inv();
}

template<class T>
std::vector<T> sharp_p_subset_sum(const std::vector<int>& values, int max_sum){
  assert(max_sum >= 0);
  std::vector<int> frequency(max_sum+1);
  int limit = 0;
  long long zeros = 0;
  for(int value : values){
    assert(value >= 0);
    if(value == 0) ++zeros;
    else if(value <= max_sum){
      ++frequency[value];
      limit += std::min(value,max_sum-limit);
    }
  }

  using FPS = FormalPowerSeries<T>;
  FPS logarithm(limit+1);
  if(limit > 0) FPS::factorials.precompute(limit);
  // log(1+x^s) = sum_{j>=1} (-1)^(j-1)*x^(s*j)/j.
  for(int s = 1; s <= limit; ++s){
    if(frequency[s] == 0) continue;
    T count = frequency[s];
    for(int j = 1, last = limit/s; j <= last; ++j){
      logarithm[s*j] += count*FPS::factorials.inv(j);
      count = -count;
    }
  }
  FPS result = logarithm.exp();
  if(zeros) result *= T(2).pow(zeros);
  result.resize(max_sum+1);
  return result;
}

template<class T>
std::vector<T> stirling_first_kind(int n){
  assert(n >= 0);
  if(n >= 2) FormalPowerSeries<T>::factorials.precompute(n/2);
  return internal::stirling_first_row<T>(n);
}

template<class T>
std::vector<T> stirling_second_kind(int n){
  assert(n >= 0);
  if(n == 0) return {T(1)};
  const auto inverses = internal::factorial_and_inverse_factorials<T>(n).second;
  auto positive = internal::integer_power_table<T>(n+1,n);
  std::vector<T> negative(n+1);
  for(int i = 0; i <= n; ++i){
    const T inverse = inverses[i];
    positive[i] *= inverse;
    negative[i] = i&1 ? -inverse : inverse;
  }
  auto result = internal::fps_product(positive,negative);
  result.resize(n+1);
  return result;
}

template<class T>
std::vector<T> stirling_first_kind_fixed_k(int n, int k){
  return internal::stirling_column<true,T>(n,k);
}

template<class T>
std::vector<T> stirling_second_kind_fixed_k(int n, int k){
  return internal::stirling_column<false,T>(n,k);
}

}  // namespace soryuusi
