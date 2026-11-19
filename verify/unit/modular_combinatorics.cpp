#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <utility>
#include <vector>

#include "library/math/binomial_coefficient.hpp"
#include "library/math/binomial_coefficient_prefix_sum.hpp"
#include "library/math/internal_math.hpp"
#include "library/math/modint.hpp"
#include "library/math/modint_q_factorials.hpp"
#include "library/math/prime_field_tables.hpp"

namespace {

void q_factorials(){
  using T = soryuusi::dynamic_modint<0>;
  for(int p : {2,3,5,11,31}){
    T::set_mod(p);
    for(int q = 0; q < p; ++q){
      soryuusi::modint_q_factorials<T> table{T(q)};
      constexpr int limit = 100;
      std::vector<T> row(limit+1), powers(limit+1,1), integers(limit+1);
      row[0] = 1;
      for(int i = 1; i <= limit; ++i){
        powers[i] = powers[i-1]*T(q);
        integers[i] = integers[i-1]*T(q)+T(1);
      }
      T factorial = 1;
      for(int n = 0; n <= limit; ++n){
        if(n){
          for(int k = n; k > 0; --k) row[k] = row[k-1]+powers[k]*row[k];
          factorial *= integers[n];
        }
        assert(table.q_integer(n) == integers[n]);
        assert(table.fact(n) == factorial);
        if(integers[n] != T(0)) assert(table.inv(n)*integers[n] == 1);
        assert(table.nCk(n,n+1) == 0);
        if(factorial != T(0)) assert(table.ifact(n)*factorial == 1);
        T falling = 1;
        for(int k = 0; k <= n; ++k){
          assert(table.nCk(n,k) == row[k] && table.nPk(n,k) == falling);
          if(row[k] != T(0)) assert(table.nCk_inv(n,k)*row[k] == 1);
          if(falling != T(0)) assert(table.nPk_inv(n,k)*falling == 1);
          if(k < n) falling *= integers[n-k];
        }
      }
      if(q){
        for(int n = -5; n < 0; ++n){
          T product = 1, denominator = 1;
          for(int k = 0; k <= 5; ++k){
            assert(table.nPk<true>(n,k) == product);
            if(denominator != T(0)) assert(table.nCk<true>(n,k) == product/denominator);
            product *= table.q_integer(n-k);
            denominator *= integers[k+1];
          }
        }
      }
    }
  }

  using M = soryuusi::modint998244353;
  const long long p = M::mod();
  soryuusi::modint_q_factorials<M> alternating(M(-1)), ordinary(M(1)), primitive(M(3));
  assert(alternating.order() == 2 && ordinary.order() == 1);
  assert(alternating.nCk(2*p+7,2*p+3) == 3);
  assert(ordinary.nCk(3*p+7,p+2) == 63);
  assert(primitive.order() == p-1 && primitive.nCk(2*(p-1)+7,(p-1)+3) == primitive.nCk(7,3)*2);
  assert(ordinary.nCk<true>(std::numeric_limits<long long>::min(),1) == M(std::numeric_limits<long long>::min()));
  using Wide = soryuusi::static_modint64<18446744073709551557ULL>;
  soryuusi::modint_q_factorials<Wide> wide(Wide(2),8);
  assert(wide.q_integer(-1) == -Wide(2).inv());
  assert(wide.nCk(5,2) == Wide(155));
}

void composite_binomials(){
  auto test = [](int mod){
    const soryuusi::BinomialCoefficient table(mod), without_table(mod,0);
    std::vector<int> row(111);
    row[0] = 1%mod;
    for(int n = 0; n <= 110; ++n){
      if(n) for(int k = n; k > 0; --k) row[k] = (std::uint64_t(row[k-1])+row[k])%mod;
      for(int k = 0; k <= n; ++k){
        assert(table.nCk(n,k) == static_cast<unsigned>(row[k]));
        assert(without_table.nCk(n,k) == static_cast<unsigned>(row[k]));
      }
    }
    constexpr auto n = std::numeric_limits<long long>::max();
    const auto choose2 = __uint128_t(n)*(n-1)/2;
    assert(table.nCk(n,2) == choose2%mod);
  };
  for(int mod = 1; mod <= 54; ++mod) test(mod);
  for(int mod : {125,343,1024,720720,1000003,2147483647}) test(mod);
  const soryuusi::BinomialCoefficient power_of_two(1024);
  assert(power_of_two.nCk(1LL<<60,1LL<<50) == 0);
  assert(power_of_two.nCk(1LL<<60,1LL<<51) == 512);
  std::mt19937_64 random(55533);
  for(int mod : {4,16,27,625,1024,720720}){
    const soryuusi::BinomialCoefficient cached(mod), uncached(mod,0);
    for(int i = 0; i < 2000; ++i){
      const long long n = random() >> 1;
      const long long k = i&1 ? n/2 : random()%n;
      assert(cached.nCk(n,k) == uncached.nCk(n,k));
    }
  }
}

void binomial_prefixes(){
  using T = soryuusi::modint998244353;
  const std::vector<std::pair<int,int>> queries{{100,99},{15,-1},{75,22},{0,0},{16,80},{100,0},{32,29},{70,13}};
  const auto result = soryuusi::BinomialCoefficientPrfixSum<T>(queries);
  for(std::size_t id = 0; id < queries.size(); ++id){
    const auto [n,k] = queries[id];
    T sum = k >= 0, choose = 1;
    for(int i = 1; i <= n && i <= k; ++i){
      choose = choose*T(n-i+1)/T(i);
      sum += choose;
    }
    assert(result[id] == sum);
  }
}

void prime_field(){
  for(int p : {2,3,17,257,1009,65537,998244353,1000000007,2147483647}){
    const soryuusi::PrimeFieldTables table(p);
    const std::uint64_t generator = table.generator();
    std::mt19937_64 random(6748);
    const int attempts = p <= 65537 ? p-1 : 10000;
    for(int i = 1; i <= attempts; ++i){
      const auto x = p <= 65537 ? i : 1+random()%(p-1);
      const auto exponent = table.log(x);
      assert(exponent < static_cast<unsigned>(p-1));
      assert(soryuusi::internal::pow_mod64_constexpr(generator,exponent,p) == x);
      const auto inverse = table.inv(x);
      assert(x*inverse%p == 1 && table.pow(x,-1) == inverse);
      const long long k = static_cast<long long>(random() >> 1);
      assert(table.pow(x,k) == soryuusi::internal::pow_mod64_constexpr(x,k,p));
    }
    assert(table.pow(0,0) == 1 && table.pow(0,p-1) == 0);
    assert(table.pow(1,std::numeric_limits<long long>::min()) == 1);
    assert(table.pow(p-1,std::numeric_limits<long long>::min()) == 1);
  }
  const soryuusi::PrimeFieldTables inverse_only(1000000007,false);
  assert(inverse_only.inv(432143125U)*432143125ULL%1000000007 == 1);
}

}  // namespace

int main(){
  q_factorials();
  composite_binomials();
  binomial_prefixes();
  prime_field();
}
