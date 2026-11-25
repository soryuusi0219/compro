// https://github.com/soryuusi0219/compro/blob/main/library/math/modint_q_factorials.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

#include "internal/internal_bit.hpp"
#include "internal/internal_math.hpp"
#include "internal/modint_traits.hpp"
#include "prime_factorization.hpp"

namespace soryuusi {

template<class T>
class modint_q_factorials {
  using Index = std::uint64_t;

  class Table {
    T next = 1;
  public:
    std::vector<T> factorial{T(1)}, inverse{T(1)};

    void extend(Index n, T q){
      const Index old = factorial.size()-1;
      if(n <= old) return;
      factorial.resize(n+1);
      inverse.resize(n+1);
      if(q == T(1)){
        for(Index i = old+1; i <= n; ++i) factorial[i] = factorial[i-1]*T::raw(i);
        inverse[n] = factorial[n].inv();
        for(Index i = n; i > old+1; --i) inverse[i-1] = inverse[i]*T::raw(i);
      }else{
        for(Index i = old+1; i <= n; ++i){
          factorial[i] = factorial[i-1]*next;
          inverse[i] = next;
          next = next*q+T(1);
        }
        T reciprocal = factorial[n].inv();
        for(Index i = n; i > old; --i){
          const T term = inverse[i];
          inverse[i] = reciprocal;
          reciprocal *= term;
        }
      }
    }
  };

  static constexpr Index automatic_limit = 10'000'000;
  const Index mod;
  const T q;
  const Index q_order, period;
  Table gaussian, ordinary;

  static Index checked_prime(){
    const Index p = T::mod();
    if constexpr (internal::is_static_modint_like_v<T>) static_assert(internal::is_prime64<T::mod()>);
    else assert(internal::is_prime64_runtime(p));
    return p;
  }

  static Index multiplicative_order(T value, Index p){
    if(value == T(0)) return 0;
    if(value == T(1)) return 1;
    Index order = p-1;
    std::vector<Index> factors;
    internal::factorize(order,factors);
    for(Index divisor : factors)
      if(order % divisor == 0 && internal::pow_mod64_constexpr(value.val(),order/divisor,p) == 1)
        order /= divisor;
    return order;
  }

  static Index prefix_limit(Index n, Index end){
    return std::min(n,end-1);
  }

  static bool ready(Table& table, Index n, Index end, T base){
    if(n < table.factorial.size()) return true;
    if(n > automatic_limit) return false;
    const Index target = std::min({end-1,automatic_limit,soryuusi::bit_ceil(n+1)-1});
    table.extend(target,base);
    return true;
  }

  T integer(Index n, T base) const {
    if(base == T(1)) return T::raw(n % mod);
    if(base == T(0)) return n ? T(1) : T(0);
    return (T::raw(internal::pow_mod64_constexpr(base.val(),n,mod))-T(1))/(base-T(1));
  }

  T product(Index n, Index k, T base) const {
    if(!k) return 1;
    T result = 1;
    if(base == T(1)){
      for(Index i = n-k+1; i <= n; ++i) result *= T::raw(i);
    }else{
      T term = integer(n-k+1,base);
      for(Index i = 0; i < k; ++i){
        result *= term;
        term = term*base+T(1);
      }
    }
    return result;
  }

  template<bool Reciprocal>
  T digit(Table& table, Index n, Index k, Index end, T base){
    k = std::min(k,n-k);
    if(!k) return 1;
    if(n < table.factorial.size() || (k > 32 && ready(table,n,end,base))){
      if constexpr (Reciprocal) return table.inverse[n]*table.factorial[k]*table.factorial[n-k];
      else return table.factorial[n]*table.inverse[k]*table.inverse[n-k];
    }
    // Reflect near p-1 so even a large ordinary digit can require only a short product.
    if(base == T(1) && mod-1-n < k){
      const Index complement = mod-1-n;
      T numerator = product(k+complement,complement,base), denominator = product(complement,complement,base);
      const T answer = Reciprocal ? denominator/numerator : numerator/denominator;
      return k&1 ? -answer : answer;
    }
    const T numerator = product(n,k,base), denominator = product(k,k,base);
    return Reciprocal ? denominator/numerator : numerator/denominator;
  }

  template<bool Reciprocal>
  T lucas(Index n, Index k, Table& table){
    T result = 1;
    while(k){
      if(n < mod) return result*digit<Reciprocal>(table,n,k,mod,T(1));
      const Index top = n%mod, bottom = k%mod;
      if(bottom > top){assert(!Reciprocal); return 0;}
      result *= digit<Reciprocal>(table,top,bottom,mod,T(1));
      n /= mod;
      k /= mod;
    }
    return result;
  }

  template<bool Reciprocal = false>
  T choose(Index n, Index k){
    if(q_order == 0 || k == 0 || k == n) return 1;
    if(q_order == 1) return lucas<Reciprocal>(n,k,gaussian);
    if(n < period) return digit<Reciprocal>(gaussian,n,k,period,q);
    const Index top = n%period, bottom = k%period;
    if(bottom > top){assert(!Reciprocal); return 0;}
    return digit<Reciprocal>(gaussian,top,bottom,period,q)*lucas<Reciprocal>(n/period,k/period,ordinary);
  }

  template<bool Reciprocal = false>
  T falling(Index n, Index k){
    if(!k || q_order == 0) return 1;
    if(n >= period){
      if(n/period != (n-k)/period){assert(!Reciprocal); return 0;}
      n %= period;
    }
    if(n < gaussian.factorial.size() || (k > 32 && ready(gaussian,n,period,q))){
      if constexpr (Reciprocal) return gaussian.inverse[n]*gaussian.factorial[n-k];
      else return gaussian.factorial[n]*gaussian.inverse[n-k];
    }
    const T result = product(n,k,q);
    return Reciprocal ? result.inv() : result;
  }

  T negative_scale(long long n, Index k) const {
    assert(q_order > 0);
    const Index magnitude = Index(0)-static_cast<Index>(n);
    const auto exponent = (__uint128_t(k)*magnitude+__uint128_t(k)*(k-1)/2)%q_order;
    const T power = T::raw(internal::pow_mod64_constexpr(q.val(),exponent ? q_order-static_cast<Index>(exponent) : 0,mod));
    return k&1 ? -power : power;
  }

  Index reduced_integer(long long n) const {
    if(static_cast<Index>(n) < period && n >= 0) return n;
    const Index magnitude = n >= 0 ? static_cast<Index>(n) : Index(0)-static_cast<Index>(n);
    const Index x = magnitude%period;
    return n < 0 && x ? period-x : x;
  }

public:
  explicit modint_q_factorials(T base, long long n = 0)
      : mod(checked_prime()), q(base), q_order(multiplicative_order(base,mod)),
        period(q_order == 1 ? mod : q_order){
    precompute(n);
  }

  Index order() const {return q_order;}

  void precompute(long long n){
    assert(n >= 0 && static_cast<Index>(T::mod()) == mod);
    if(q_order == 0) return;
    gaussian.extend(prefix_limit(n,period),q);
    if(q_order > 1 && static_cast<Index>(n) >= period)
      ordinary.extend(prefix_limit(n/period,mod),T(1));
  }

  T q_integer(long long n) const {
    assert(static_cast<Index>(T::mod()) == mod);
    if(q_order == 0){
      assert(n >= 0);
      return n ? T(1) : T(0);
    }
    const Index x = reduced_integer(n);
    if(x && x < gaussian.factorial.size()) return gaussian.factorial[x]*gaussian.inverse[x-1];
    return integer(x,q);
  }

  T mint_inv(long long n){
    assert(static_cast<Index>(T::mod()) == mod);
    if(q_order == 0){assert(n > 0); return 1;}
    const Index x = reduced_integer(n);
    assert(x != 0);
    if(ready(gaussian,x,period,q)) return gaussian.factorial[x-1]*gaussian.inverse[x];
    return integer(x,q).inv();
  }

  T mint_factorial(long long n){
    assert(n >= 0 && static_cast<Index>(T::mod()) == mod);
    if(q_order == 0) return 1;
    const Index x = n;
    if(x >= period) return 0;
    if(ready(gaussian,x,period,q)) return gaussian.factorial[x];
    return product(x,x,q);
  }

  T mint_inv_factorial(long long n){
    assert(n >= 0 && static_cast<Index>(T::mod()) == mod);
    if(q_order == 0) return 1;
    const Index x = n;
    assert(x < period);
    if(ready(gaussian,x,period,q)) return gaussian.inverse[x];
    return product(x,x,q).inv();
  }

  T inv(long long n) {return mint_inv(n);}
  T fact(long long n) {return mint_factorial(n);}
  T ifact(long long n) {return mint_inv_factorial(n);}

  template<bool admit_negative = false>
  T nCk(long long n, long long k){
    assert(static_cast<Index>(T::mod()) == mod);
    if(k < 0) return 0;
    if constexpr (admit_negative) {
      if(n < 0) return k == 0 ? T(1) : negative_scale(n,k)*choose(Index(k)-Index(n)-1,k);
    }
    assert(n >= 0);
    return k > n ? T(0) : choose(n,k);
  }

  template<bool admit_negative = false>
  T nPk(long long n, long long k){
    assert(static_cast<Index>(T::mod()) == mod);
    if(k < 0) return 0;
    if constexpr (admit_negative) {
      if(n < 0) return k == 0 ? T(1) : negative_scale(n,k)*falling(Index(k)-Index(n)-1,k);
    }
    assert(n >= 0);
    return k > n ? T(0) : falling(n,k);
  }

  template<bool admit_negative = false>
  T nCk_inv(long long n, long long k){
    assert(k >= 0 && static_cast<Index>(T::mod()) == mod);
    if constexpr (admit_negative) {
      if(n < 0) return k == 0 ? T(1) : choose<true>(Index(k)-Index(n)-1,k)/negative_scale(n,k);
    }
    assert(0 <= k && k <= n);
    return choose<true>(n,k);
  }
  template<bool admit_negative = false>
  T nPk_inv(long long n, long long k){
    assert(k >= 0 && static_cast<Index>(T::mod()) == mod);
    if constexpr (admit_negative) {
      if(n < 0) return k == 0 ? T(1) : falling<true>(Index(k)-Index(n)-1,k)/negative_scale(n,k);
    }
    assert(0 <= k && k <= n);
    return falling<true>(n,k);
  }
};

}  // namespace soryuusi
