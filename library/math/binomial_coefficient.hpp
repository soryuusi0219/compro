// https://github.com/soryuusi0219/compro/blob/main/library/math/binomial_coefficient.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>

#include "internal/internal_bit.hpp"
#include "internal/internal_math.hpp"
#include "prime_factorization.hpp"

namespace soryuusi {

// Binomial coefficients for one fixed modulus, including composite moduli.
class BinomialCoefficient {
  using U64 = std::uint64_t;
  using U32 = std::uint32_t;

  class Divider {
    const U64 divisor, reciprocal;
    const int shift;
  public:
    explicit Divider(U32 d)
        : divisor(d), reciprocal(~U64(0)/d+1),
          shift(soryuusi::has_single_bit(d) ? soryuusi::countr_zero(d) : -1) {}

    std::pair<U64,U32> split(U64 x) const {
      if(shift >= 0) return {x >> shift,x & (divisor-1)};
      U64 quotient = (__uint128_t(x)*reciprocal) >> 64;
      const U64 product = quotient*divisor;
      if(product > x) return {quotient-1,x+divisor-product};
      return {quotient,x-product};
    }
  };

  class PrimePower {
    const U32 p, pk;
    const int exponent;
    const Divider by_p, by_pk;
    const internal::barrett reduce;
    std::vector<U32> factorial, inverse, powers;

    U32 apply_valuation(U32 value, int valuation) const {
      return valuation >= exponent ? 0 : reduce.mul(value,powers[valuation]);
    }

    template<bool Cached>
    U32 prime_digit(U32 n, U32 k) const {
      if constexpr (Cached) return reduce.mul(factorial[n],reduce.mul(inverse[k],inverse[n-k]));
      else{
        k = std::min(k,n-k);
        const U32 reflected = p-1-n;
        const bool negative = reflected < k && (k&1);
        if(reflected < k){
          n = k+reflected;
          k = reflected;
        }
        U32 numerator = 1, denominator = 1;
        for(U32 i = 1; i <= k; ++i){
          numerator = reduce.mul(numerator,n-k+i);
          denominator = reduce.mul(denominator,i);
        }
        const U32 value = reduce.mul(numerator,internal::inv_gcd64(denominator,p).second);
        return negative ? p-value : value;
      }
    }

    template<bool Cached>
    U32 prime_digits(U64 n, U64 k) const {
      U32 result = 1;
      while(k){
        if(n < p) return reduce.mul(result,prime_digit<Cached>(n,k));
        const auto [next_n,a] = by_p.split(n);
        const auto [next_k,b] = by_p.split(k);
        if(b > a) return 0;
        result = reduce.mul(result,prime_digit<Cached>(a,b));
        n = next_n;
        k = next_k;
      }
      return result;
    }

    template<class Emit>
    std::pair<int,bool> prime_power_digits(U64 n, U64 k, Emit emit) const {
      U64 r = n-k;
      int valuation = 0;
      bool negative = false;
      const bool negative_cycle = p != 2 || exponent == 2;
      // Remove powers of p from each factorial, one p-adic digit at a time.
      while(n != r){
        const auto next_n = by_p.split(n).first;
        const auto next_k = by_p.split(k).first;
        const auto next_r = by_p.split(r).first;
        valuation += next_n-next_k-next_r;
        if(valuation >= exponent) return {valuation,false};
        if(n < pk){
          emit(n,k,r);
        }else{
          const auto [whole_n,a] = by_pk.split(n);
          const auto [whole_k,b] = by_pk.split(k);
          const auto [whole_r,c] = by_pk.split(r);
          negative ^= negative_cycle && ((whole_n^whole_k^whole_r)&1);
          emit(a,b,c);
        }
        n = next_n;
        k = next_k;
        r = next_r;
      }
      return {valuation,negative};
    }

    U32 prefixes_on_demand(U64 n, U64 k) const {
      std::vector<std::pair<U32,bool>> prefixes;
      bool reflected_sign = false;
      auto append = [&](U32 index, bool denominator){
        if(index > pk/2){
          index = pk-1-index;
          denominator = !denominator;
          reflected_sign ^= (p != 2 || exponent == 2) ^ ((index-index/p)&1);
        }
        if(index) prefixes.emplace_back(index,denominator);
      };
      const auto [valuation,negative] = prime_power_digits(n,k,[&](U32 a, U32 b, U32 c){
        append(a,false);
        append(b,true);
        append(c,true);
      });
      if(valuation >= exponent) return 0;
      std::sort(prefixes.begin(),prefixes.end());
      U32 numerator = 1, denominator = 1, prefix = 1, i = 1, digit = 1;
      for(auto [last,divide] : prefixes){
        for(; i <= last; ++i){
          if(digit == p) digit = 1;
          else {prefix = reduce.mul(prefix,i); ++digit;}
        }
        if(divide) denominator = reduce.mul(denominator,prefix);
        else numerator = reduce.mul(numerator,prefix);
      }
      const U32 value = apply_valuation(reduce.mul(numerator,internal::inv_gcd64(denominator,pk).second),valuation);
      return negative != reflected_sign ? pk-value : value;
    }

    U32 small_product(U64 n, U64 k) const {
      if(k > 128){
        const auto [valuation,negative] = prime_power_digits(n,k,[](U32, U32, U32){});
        if(valuation >= exponent) return 0;
      }
      int valuation = 0;
      U32 numerator = 1, denominator = 1;
      for(U64 i = 1; i <= k; ++i){
        U64 a = n-k+i, b = i;
        for(auto [quotient,remainder] = by_p.split(a); !remainder; std::tie(quotient,remainder) = by_p.split(a)){
          a = quotient;
          ++valuation;
        }
        for(auto [quotient,remainder] = by_p.split(b); !remainder; std::tie(quotient,remainder) = by_p.split(b)){
          b = quotient;
          --valuation;
        }
        numerator = reduce.mul(numerator,by_pk.split(a).second);
        denominator = reduce.mul(denominator,by_pk.split(b).second);
      }
      if(valuation >= exponent) return 0;
      return apply_valuation(reduce.mul(numerator,internal::inv_gcd64(denominator,pk).second),valuation);
    }

    U32 binary_digits(U64 n, U64 k, int valuation) const {
      U64 r = n-k;
      U32 value = 1;
      bool negative = false;
      while(n != r){
        const U32 a = n&(pk-1), b = k&(pk-1), c = r&(pk-1);
        // Unsigned wrap preserves the low bits modulo the power of two.
        value = value*factorial[a]*inverse[b]*inverse[c] & (pk-1);
        if(exponent == 2) negative ^= ((n^k^r)&pk) != 0;
        n >>= 1;
        k >>= 1;
        r >>= 1;
      }
      const U32 result = (value << valuation) & (pk-1);
      return negative ? pk-result : result;
    }

  public:
    PrimePower(U32 prime, int e, U32 modulus, U32 table_limit)
        : p(prime), pk(modulus), exponent(e), by_p(p), by_pk(pk), reduce(pk), powers(e,1){
      for(int i = 1; i < exponent; ++i) powers[i] = powers[i-1]*p;
      if(pk > table_limit) return;
      factorial.resize(pk,1);
      inverse.resize(pk,1);
      for(U32 i = 1, digit = 1; i < pk; ++i){
        if(digit == p){
          factorial[i] = factorial[i-1];
          digit = 1;
        }else{
          factorial[i] = reduce.mul(factorial[i-1],i);
          ++digit;
        }
      }
      inverse[pk-1] = internal::inv_gcd64(factorial[pk-1],pk).second;
      for(U32 i = pk-1, digit = p-1; i > 0; --i){
        if(digit == 0){
          inverse[i-1] = inverse[i];
          digit = p-1;
        }else{
          inverse[i-1] = reduce.mul(inverse[i],i);
          --digit;
        }
      }
    }

    U32 choose(U64 n, U64 k) const {
      if(exponent == 1){
        if(p == 2) return (k&~n) == 0;
        return factorial.empty() ? prime_digits<false>(n,k) : prime_digits<true>(n,k);
      }
      if(p == 2){
        const int valuation = soryuusi::popcount(k)+soryuusi::popcount(n-k)-soryuusi::popcount(n);
        if(valuation >= exponent) return 0;
        if(!factorial.empty()) return binary_digits(n,k,valuation);
      }
      if(factorial.empty()) return k > pk/2 ? prefixes_on_demand(n,k) : small_product(n,k);
      U32 product = 1;
      const auto [valuation,negative] = prime_power_digits(n,k,[&](U32 a, U32 b, U32 c){
        product = reduce.mul(product,reduce.mul(factorial[a],reduce.mul(inverse[b],inverse[c])));
      });
      if(valuation >= exponent) return 0;
      const U32 value = apply_valuation(product,valuation);
      return negative ? pk-value : value;
    }
  };

  const U32 modulus;
  const internal::barrett reduce;
  std::vector<PrimePower> factors;
  std::vector<U32> weights;

public:
  explicit BinomialCoefficient(int mod, U32 table_limit = 1'000'000)
      : modulus(mod), reduce(mod > 0 ? mod : 1){
    assert(mod > 0);
    for(auto [p,e] : prime_factorization_paired(mod)){
      U32 pk = 1;
      for(int i = 0; i < e; ++i) pk *= p;
      factors.emplace_back(p,e,pk,table_limit);
      const U32 complement = modulus/pk;
      weights.push_back(reduce.mul(complement,internal::inv_gcd64(complement,pk).second));
    }
  }

  U32 mod() const {return modulus;}

  U32 nCk(long long n, long long k) const {
    assert(n >= 0);
    if(modulus == 1 || k < 0 || k > n) return 0;
    k = std::min(k,n-k);
    if(k == 0) return 1;
    if(k == 1) return n%modulus;
    if(factors.size() == 1) return factors[0].choose(n,k);
    // The sum of the prime powers is at most their product, modulus.
    U64 result = 0;
    for(std::size_t i = 0; i < factors.size(); ++i)
      result += U64(factors[i].choose(n,k))*weights[i];
    return result < modulus ? result : result%modulus;
  }
};

}  // namespace soryuusi
