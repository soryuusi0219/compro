#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

#include "library/math/binomial_coefficient_prefix_sum.hpp"
#include "library/math/convolution_plus_max.hpp"
#include "library/math/gf2.hpp"
#include "library/math/matrix.hpp"
#include "library/math/modint.hpp"
#include "library/math/nimber.hpp"
#include "library/math/number_of_increasing_sequence.hpp"

namespace {

using mint = soryuusi::modint998244353;

void test_binomial_prefix_sum() {
  std::vector<std::pair<int,int>> queries;
  for(int n = 0; n <= 20; ++n) for(int k = 0; k <= n; ++k) queries.emplace_back(n,k);
  auto result = soryuusi::BinomialCoefficientPrfixSum<mint>(queries);
  for(int q = 0; q < (int)queries.size(); ++q) {
    auto [n,k] = queries[q];
    mint expected = 0, choose = 1;
    for(int i = 0; i <= k; ++i) {
      if(i) choose *= mint(n-i+1)/i;
      expected += choose;
    }
    assert(result[q] == expected);
  }
}

void test_plus_max_convolution() {
  std::vector<long long> a{4,1,7,2}, b{3,8,5};
  auto result = soryuusi::plus_max_convolution(a,b);
  std::vector<long long> expected(a.size()+b.size()-1);
  for(int i = 0; i < (int)a.size(); ++i) {
    for(int j = 0; j < (int)b.size(); ++j) expected[i+j] += std::max(a[i],b[j]);
  }
  assert(result == expected);
  a = {-4,1,-7,2}; b = {-3,-8,5};
  result = soryuusi::plus_max_convolution(a,b);
  expected.assign(a.size()+b.size()-1,0);
  for(int i = 0; i < (int)a.size(); ++i) {
    for(int j = 0; j < (int)b.size(); ++j) expected[i+j] += std::max(a[i],b[j]);
  }
  assert(result == expected);
  assert(soryuusi::plus_max_convolution(std::vector<int>{},std::vector<int>{1}).empty());
}

void test_gf2() {
  using F = soryuusi::GF2<8>;
  for(int a = 0; a < 32; ++a) for(int b = 0; b < 32; ++b) {
    assert(F(a)+F(b) == F(a^b));
    assert(F(a)*F(b) == F(b)*F(a));
    if(b) assert((F(a)/F(b))*F(b) == F(a));
  }
  for(int a = 0; a < 32; ++a) assert(F(a).sqrt()*F(a).sqrt() == F(a));
}

void test_matrix() {
  soryuusi::Matrix<mint> a{{1,2},{3,4}};
  soryuusi::Matrix<mint> b{{5,6},{7,8}};
  assert((a*b == soryuusi::Matrix<mint>{{19,22},{43,50}}));
  assert((-a == soryuusi::Matrix<mint>{{-1,-2},{-3,-4}}));
  assert(a.det() == mint(-2));
  assert(a*a.inv() == soryuusi::Matrix<mint>::I(2));
  assert((soryuusi::Matrix<mint>{{1,1},{1,0}}.pow(10)[0][1] == mint(55)));
}

void test_nimber() {
  using N = soryuusi::Nimber;
  assert((N(2)*N(2)).get() == 3);
  assert((N(2)*N(3)).get() == 1);
  assert((N(3)*N(3)).get() == 2);
  for(unsigned long long a = 0; a < 32; ++a) {
    for(unsigned long long b = 1; b < 32; ++b) {
      assert(((N(a)/N(b))*N(b)).get() == a);
      assert((N(a)+N(b)).get() == (a^b));
    }
  }
}

long long count_sequences(const std::vector<long long>& bound, int m) {
  const int n = bound.size();
  long long result = 0;
  auto dfs = [&](auto&& self, int i, int last) -> void {
    if(i == n) {++result; return;}
    for(int x = last; x < m && x < bound[i]; ++x) self(self,i+1,x);
  };
  dfs(dfs,0,0);
  return result;
}

void test_number_of_increasing_sequences() {
  for(int n = 1; n <= 5; ++n) for(int m = 1; m <= 6; ++m) {
    std::vector<long long> bound(n);
    for(int i = 0; i < n; ++i) bound[i] = std::min(m,i+2);
    auto result = soryuusi::num_inc_seq_under<mint>(n,m,bound);
    assert(result == mint(count_sequences(bound,m)));
  }
  assert(soryuusi::num_inc_seq_under<mint>(0,0,{}) == mint(1));
}

}  // namespace

int main() {
  test_binomial_prefix_sum();
  test_plus_max_convolution();
  test_gf2();
  test_matrix();
  test_nimber();
  test_number_of_increasing_sequences();
  std::cout << "math_misc: all tests passed\n";
}
