// https://github.com/soryuusi0219/compro/blob/main/library/math/prime_field_tables.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "internal/internal_bit.hpp"
#include "../data_structure/hash_table.hpp"
#include "internal/internal_math.hpp"
#include "linear_sieve.hpp"
#include "prime_factorization.hpp"

namespace soryuusi {

class PrimeFieldTables {
  using U32 = std::uint32_t;
  using U64 = std::uint64_t;

  struct Fraction {
    U32 numerator, denominator;
    bool negative;
  };

  const U32 p, group;
  const internal::barrett reduce, reduce_exponent;
  U32 root = 0;
  int bucket_shift = 0, power_shift = 0;
  std::vector<std::uint16_t> denominators;
  std::vector<U32> inverses, logarithms, powers_low, powers_high;

  static U32 checked_prime(int modulus){
    assert(modulus >= 2 && internal::is_prime64_runtime(modulus));
    return modulus;
  }

  U32 add_exponents(U32 a, U32 b) const {
    U32 value = a+b;
    return value >= group ? value-group : value;
  }

  U32 subtract_exponents(U32 a, U32 b) const {
    return a >= b ? a-b : a+group-b;
  }

  U32 power(U32 x, U32 n) const {
    U32 y = 1;
    for(; n; n >>= 1){
      if(n&1) y = reduce.mul(y,x);
      if(n > 1) x = reduce.mul(x,x);
    }
    return y;
  }

  U32 make_buckets(){
    const U32 cube_root = std::max(2,static_cast<int>(std::cbrt(p)));
    const U32 step = soryuusi::bit_floor(std::max<U32>(1,p/(cube_root*cube_root)));
    const U32 order = std::sqrt(p/step);
    bucket_shift = soryuusi::countr_zero(step);
    denominators.resize((p+step-1)/step);
    U32 a = 0, b = 1, c = 1, d = order;
    U32 j = 0;
    while(j < denominators.size()){
      while(j < denominators.size()){
        const U32 midpoint = std::min(p-1,(j << bucket_shift)+step/2);
        if(U64(midpoint)*d > U64(p)*c) break;
        denominators[j++] = std::min(b,d);
      }
      if(j == denominators.size()) break;
      const U32 k = (order+b)/d;
      const U32 next_a = k*c-a, next_b = k*d-b;
      a = c; b = d;
      c = next_a; d = next_b;
    }
    // Farey neighbors give |b*x-a*p| <= 2*p/order at the bucket midpoint.
    return std::min<U64>(p/2,(2*p+U64(order)-1)/order+U64(order)*(step/2));
  }

  Fraction decompose(U32 x) const {
    if(x < inverses.size()) return {x,1,false};
    const U32 b = denominators[x >> bucket_shift];
    const U32 a = reduce.mul(x,b);
    if(a > p/2) return {p-a,b,true};
    return {a,b,false};
  }

  void find_generator(const std::vector<std::pair<long long,long long>>& factors){
    if(p == 2){
      root = 1;
      return;
    }
    for(root = 2;; ++root){
      bool good = true;
      for(auto [prime,e] : factors)
        if(power(root,group/prime) == 1){good = false; break;}
      if(good) return;
    }
  }

  // Recover the logs of small primes modulo one prime power by digit lifting.
  void add_subgroup_logs(U32 prime, int exponent, const std::vector<int>& primes){
    U32 pk = 1;
    for(int i = 0; i < exponent; ++i) pk *= prime;
    const U32 projection = group/pk;
    const U32 weight = reduce_exponent.mul(projection,internal::inv_gcd64(projection,pk).second);
    const U32 generator = power(root,group/prime);
    std::vector<U32> inverse_bases(exponent);
    inverse_bases[0] = power(root,group-projection);
    for(int i = 1; i < exponent; ++i) inverse_bases[i] = power(inverse_bases[i-1],prime);

    const U32 width = std::min<U32>(prime,std::sqrt(double(prime)*std::max<std::size_t>(1,primes.size()))+1);
    auto solve = [&](auto discrete_digit){
      for(U32 v : primes){
        U32 residue = power(v,projection), partial = 0, place = 1, rest = pk/prime;
        for(int i = 0; i < exponent; ++i){
          const U32 digit = discrete_digit(power(residue,rest));
          partial += digit*place;
          if(i+1 < exponent && digit) residue = reduce.mul(residue,power(inverse_bases[i],digit));
          place *= prime;
          rest /= prime;
        }
        logarithms[v] = add_exponents(logarithms[v],reduce_exponent.mul(partial,weight));
      }
    };

    if(prime <= 32){
      std::vector<U32> values(prime,1);
      for(U32 i = 1; i < prime; ++i) values[i] = reduce.mul(values[i-1],generator);
      solve([&](U32 x){return std::find(values.begin(),values.end(),x)-values.begin();});
      return;
    }

    HashTable<U32,U32,hash_int<U32>> lookup(width);
    for(U32 i = 0, v = 1; i < width; ++i, v = reduce.mul(v,generator)) lookup[v] = i;
    const U32 stride_inverse = power(generator,prime-width);
    solve([&](U32 x){
      for(U32 offset = 0;; offset += width, x = reduce.mul(x,stride_inverse)){
        const U32 found = lookup.get(x,prime);
        if(found != prime) return found+offset;
      }
    });
  }

  void build_logarithms(U32 limit, bool full){
    const auto factors = prime_factorization_paired(group);
    find_generator(factors);
    logarithms.resize(limit+1);
    if(full){
      powers_low.resize(group);
      for(U32 i = 0, x = 1; i < group; ++i, x = reduce.mul(x,root)){
        logarithms[x] = i;
        powers_low[i] = x;
      }
      return;
    }

    const int small = std::sqrt(p);
    const Linear_Sieve sieve(small+1);
    for(auto [prime,e] : factors) add_subgroup_logs(prime,e,sieve.prime_table());
    for(int i = 2; i <= small; ++i){
      const int d = sieve[i];
      if(d != i) logarithms[i] = add_exponents(logarithms[d],logarithms[i/d]);
    }
    for(U32 i = small+1; i <= limit; ++i)
      logarithms[i] = subtract_exponents(add_exponents(group/2,logarithms[p%i]),logarithms[p/i]);

    power_shift = (soryuusi::bit_width(group-1)+1)/2;
    const U32 width = 1U << power_shift;
    powers_low.resize(width,1);
    powers_high.resize((group+width-1)/width,1);
    for(U32 i = 1; i < width; ++i) powers_low[i] = reduce.mul(powers_low[i-1],root);
    const U32 stride = reduce.mul(powers_low.back(),root);
    for(U32 i = 1; i < powers_high.size(); ++i) powers_high[i] = reduce.mul(powers_high[i-1],stride);
  }

public:
  explicit PrimeFieldTables(int modulus, bool with_logarithms = true)
      : p(checked_prime(modulus)), group(p-1), reduce(p), reduce_exponent(group){
    constexpr U32 full_table_limit = 1U << 16;
    const bool full = p <= full_table_limit;
    const U32 limit = full ? p-1 : make_buckets();
    inverses.resize(limit+1);
    inverses[1] = 1;
    for(U32 i = 2; i <= limit; ++i) inverses[i] = reduce.mul(p-p/i,inverses[p%i]);
    if(with_logarithms) build_logarithms(limit,full);
  }

  U32 mod() const {return p;}
  U32 generator() const {assert(root); return root;}

  U32 inv(U32 x) const {
    assert(0 < x && x < p);
    const auto [a,b,negative] = decompose(x);
    const U32 value = b == 1 ? inverses[a] : reduce.mul(inverses[a],b);
    return negative ? p-value : value;
  }

  U32 log(U32 x) const {
    assert(0 < x && x < p && root);
    const auto [a,b,negative] = decompose(x);
    U32 value = subtract_exponents(logarithms[a],logarithms[b]);
    return negative ? add_exponents(value,group/2) : value;
  }

  U32 pow(U32 x, long long n) const {
    assert(x < p && root && (x || n >= 0));
    if(!x) return n == 0;
    long long exponent = static_cast<U64>(n) < group ? n : n % static_cast<long long>(group);
    if(exponent < 0) exponent += group;
    const U32 index = reduce_exponent.mul(log(x),exponent);
    if(powers_high.empty()) return powers_low[index];
    const U32 mask = (1U << power_shift)-1;
    return reduce.mul(powers_low[index&mask],powers_high[index >> power_shift]);
  }
};

}  // namespace soryuusi
