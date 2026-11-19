#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

#include "library/math/dirichlet_sum.hpp"
#include "library/math/modint.hpp"

namespace {

long long divisor_count(int n) {
  long long result = 0;
  for(int d = 1; d <= n; ++d) result += n%d == 0;
  return result;
}

template<class T>
std::vector<T> brute_product(const std::vector<T>& a, const std::vector<T>& b){
  int n = (int)a.size()-1;
  std::vector<T> product(n+1);
  for(int i = 1; i <= n; ++i) for(int j = 1; i*j <= n; ++j) product[i*j] += a[i]*b[j];
  return product;
}

template<class DS, class T>
DS from_coefficients(const std::vector<T>& coefficients, bool multiplicative = false){
  std::vector<T> prefix = coefficients;
  for(int i = 1; i < (int)prefix.size(); ++i) prefix[i] += prefix[i-1];
  std::vector<T> lower(coefficients.begin()+1,coefficients.begin()+DS::K+1), upper;
  for(long long x : DS::eval_points) upper.push_back(prefix[x]);
  return DS(std::move(lower),std::move(upper),multiplicative);
}

template<class DS, class T>
void check(const DS& actual, const std::vector<T>& coefficients){
  auto expected = from_coefficients<DS>(coefficients);
  assert(actual.al == expected.al && actual.au == expected.au);
}

void check_products(){
  using Mint = soryuusi::modint998244353;
  using DS = soryuusi::Dirichlet_Sum<Mint>;
  std::mt19937 rng(63078);
  for(int trial = 0; trial < 70; ++trial){
    int n = trial == 0 ? 1 : 1+rng()%6000;
    int cutoff = trial%3 == 0 ? 0 : trial%3 == 1 ? rng()%(n+1) : n+5;
    DS::init(n,cutoff);
    auto primes = DS::primes;
    DS::init(n,cutoff);
    assert(DS::primes == primes);
    int limit = std::max<long long>(n,DS::K);
    std::vector<Mint> a(limit+1,1), b(limit+1,1), one(limit+1,1), linear(limit+1);
    a[0] = b[0] = one[0] = 0;
    for(int i = 1; i <= limit; ++i){
      a[i] = rng();
      if(i > 1) b[i] = rng();
      linear[i] = i;
    }
    b[1] = 2;
    if(a[1] == 0) a[1] = 1;
    auto f = from_coefficients<DS>(a), g = from_coefficients<DS>(b);
    auto zeta = DS::zeta(), power = DS::zetashift(1);
    auto convolution = f*g;
    check(convolution,brute_product(a,b));
    check(convolution/g,a);
    check(f*f,brute_product(a,a));
    check(f*zeta,brute_product(a,one));
    check(zeta*f,brute_product(one,a));
    check((f*zeta)/zeta,a);
    auto multiplicative = zeta*power;
    assert(multiplicative.is_multiplicative);
    check(multiplicative,brute_product(one,linear));
    check(multiplicative/zeta,linear);
    for(int exponent = 2; exponent <= 3; ++exponent){
      std::vector<Mint> perfect_powers(limit+1);
      for(int i = 1; ; ++i){
        long long j = exponent == 2 ? 1LL*i*i : 1LL*i*i*i;
        if(j > limit) break;
        perfect_powers[j] = 1;
      }
      check(DS::zetak(exponent),perfect_powers);
    }
  }
}

void check_powers(){
  using Mint = soryuusi::static_modint<8>;
  using DS = soryuusi::Dirichlet_Sum<Mint>;
  constexpr int n = 300;
  DS::init(n,0);
  for(int constant : {0,2,3}){
    std::vector<Mint> coefficients(n+1);
    coefficients[1] = constant;
    for(int i = 2; i <= n; ++i) coefficients[i] = i;
    auto f = from_coefficients<DS>(coefficients);
    for(long long k : {0LL,1LL,3LL,33LL,1'000'000'037LL}){
      std::vector<Mint> expected(n+1), base = coefficients;
      expected[1] = 1;
      for(long long exp = k; exp; exp >>= 1){
        if(exp&1) expected = brute_product(expected,base);
        base = brute_product(base,base);
      }
      check(f.pow(k),expected);
    }
  }
}

void check_integer_middle(){
  using DS = soryuusi::Dirichlet_Sum<long long,1>;
  constexpr int n = 65'521;
  DS::init(n,0);
  std::vector<long long> a(n+1), b(n+1);
  for(int i = 1; i <= n; ++i){
    a[i] = i%5-2;
    b[i] = i%7-3;
  }
  b[1] = 1;
  auto f = from_coefficients<DS>(a), g = from_coefficients<DS>(b);
  auto product = f*g;
  check(product,brute_product(a,b));
  check(product/g,a);
}

}  // namespace

int main() {
  using DS = soryuusi::Dirichlet_Sum<long long,0>;
  constexpr int n = 100;
  DS::init(n,10);
  const auto zeta = DS::zeta();
  const auto square = zeta*zeta;
  auto [lower,upper] = square.getAll();
  for(int i = 1; i <= 10; ++i) assert(lower[i-1] == divisor_count(i));
  long long expected = 0;
  for(int i = 1; i <= n; ++i) expected += divisor_count(i);
  assert(square.getN() == expected);
  assert((square/zeta).getN() == zeta.getN());
  assert((zeta*DS::e()).getN() == zeta.getN());
  assert(upper.front() == expected);
  check_products();
  check_powers();
  check_integer_middle();
  std::cout << "dirichlet_sum: all tests passed\n";
}
