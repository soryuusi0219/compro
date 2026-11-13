#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

#include "library/math/binomial_coefficient_prefix_sum.hpp"
#include "library/math/convolution_plus_max.hpp"
#include "library/math/gf2.hpp"
#include "library/math/matrix.hpp"
#include "library/math/matrix_characteristic_polynomial.hpp"
#include "library/math/matrix_det_arbitrary_mod.hpp"
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

long long naive_determinant(const soryuusi::Matrix<long long>& matrix) {
  int n = matrix.height();
  std::vector<int> permutation(n);
  std::iota(permutation.begin(),permutation.end(),0);
  long long result = 0;
  do {
    long long term = 1;
    int inversions = 0;
    for(int i = 0; i < n; ++i) {
      term *= matrix[i][permutation[i]];
      for(int j = 0; j < i; ++j) inversions += permutation[j] > permutation[i];
    }
    result += inversions&1 ? -term : term;
  } while(std::next_permutation(permutation.begin(),permutation.end()));
  return result;
}

soryuusi::Matrix<mint> naive_adjugate(const soryuusi::Matrix<mint>& matrix) {
  int n = matrix.height();
  soryuusi::Matrix<mint> result(n,n);
  for(int i = 0; i < n; ++i) for(int j = 0; j < n; ++j) {
    soryuusi::Matrix<mint> minor(n-1,n-1);
    for(int row = 0, ii = 0; row < n; ++row) {
      if(row == j) continue;
      for(int column = 0, jj = 0; column < n; ++column) {
        if(column == i) continue;
        minor[ii][jj++] = matrix[row][column];
      }
      ++ii;
    }
    result[i][j] = ((i+j)&1 ? -minor.det() : minor.det());
  }
  return result;
}

void test_matrix() {
  soryuusi::Matrix<mint> a{{1,2},{3,4}};
  soryuusi::Matrix<mint> b{{5,6},{7,8}};
  assert((a*b == soryuusi::Matrix<mint>{{19,22},{43,50}}));
  assert((-a == soryuusi::Matrix<mint>{{-1,-2},{-3,-4}}));
  assert(a.det() == mint(-2));
  auto echelon = a;
  assert(echelon.Gaussian_elimination(false) == mint(-2));
  assert(a.rank() == 2);
  assert((soryuusi::Matrix<mint>{{1,2},{2,4}}.rank() == 1));
  assert(a*a.inv() == soryuusi::Matrix<mint>::I(2));
  assert(a.try_inv().has_value());
  assert(a.is_invertible());
  assert((!soryuusi::Matrix<mint>{{1,2},{2,4}}.try_inv().has_value()));
  assert((!soryuusi::Matrix<mint>{{1,2,3},{4,5,6}}.is_invertible()));
  assert((soryuusi::Matrix<mint>{{1,1},{1,0}}.pow(10)[0][1] == mint(55)));

  soryuusi::Matrix<mint> rectangular{{1,2,3},{4,5,6}};
  assert((rectangular.transpose() == soryuusi::Matrix<mint>{{1,4},{2,5},{3,6}}));
  assert((rectangular*std::vector<mint>{1,0,-1} == std::vector<mint>{-2,-2}));
  assert((std::vector<mint>{2,3}*rectangular == std::vector<mint>{14,19,24}));
  assert((rectangular.submatrix(0,2,1,3) == soryuusi::Matrix<mint>{{2,3},{5,6}}));
  assert((soryuusi::Matrix<mint>::horizontal_concat(a,b) ==
          soryuusi::Matrix<mint>{{1,2,5,6},{3,4,7,8}}));
  assert((soryuusi::Matrix<mint>::vertical_concat(a,b) ==
          soryuusi::Matrix<mint>{{1,2},{3,4},{5,6},{7,8}}));

  soryuusi::Matrix<mint> reducible{{1,2,3},{2,4,7}};
  auto elimination = reducible.row_reduce();
  assert(elimination.rank == 2);
  assert((elimination.pivot_columns == std::vector<int>{0,2}));
  assert((reducible == soryuusi::Matrix<mint>{{1,2,0},{0,0,1}}));

  soryuusi::Matrix<mint> system{{1,2,3},{2,4,6}};
  auto solutions = system.solve({4,8});
  assert(solutions.has_solution && solutions.rank == 1);
  assert(solutions.nullspace_basis.size() == 2);
  assert(system*solutions.particular_solution == std::vector<mint>({4,8}));
  for(const auto& basis : solutions.nullspace_basis) {
    assert(system*basis == std::vector<mint>({0,0}));
  }
  assert(!system.solve({4,9}).has_solution);

  auto lu = a.lu_decompose();
  assert(lu.invertible && lu.det() == a.det());
  assert((lu.solve({5,11}) == std::vector<mint>{1,2}));
  auto singular_lu = soryuusi::Matrix<mint>{{1,2},{2,4}}.lu_decompose();
  assert(!singular_lu.invertible && !singular_lu.try_solve({1,2}).has_value());
  unsigned lu_seed = 987654321;
  for(int n = 0; n <= 6; ++n) for(int testcase = 0; testcase < 40; ++testcase) {
    soryuusi::Matrix<mint> random_matrix(n,n);
    for(auto& row : random_matrix) for(auto& x : row) {
      lu_seed = lu_seed*1103515245+12345;
      x = int((lu_seed>>16)%11)-5;
    }
    auto random_lu = random_matrix.lu_decompose();
    assert(random_lu.det() == random_matrix.det());
    if(!random_lu.invertible) continue;
    std::vector<mint> expected(n);
    for(auto& x : expected) {
      lu_seed = lu_seed*1103515245+12345;
      x = int((lu_seed>>16)%11)-5;
    }
    auto rhs = random_matrix*expected;
    assert(random_lu.solve(rhs) == expected);
  }

  assert((soryuusi::characteristic_polynomial(a) == std::vector<mint>{-2,-5,1}));
  assert((soryuusi::characteristic_polynomial(soryuusi::Matrix<mint>(0,0)) ==
          std::vector<mint>{1}));

  assert((a.adjugate() == soryuusi::Matrix<mint>{{4,-2},{-3,1}}));
  assert((soryuusi::Matrix<mint>{{1,2},{2,4}}.adjugate() ==
          soryuusi::Matrix<mint>{{4,-2},{-2,1}}));
  assert((soryuusi::Matrix<mint>(3,3).adjugate() == soryuusi::Matrix<mint>(3,3)));
  unsigned adjugate_seed = 314159265;
  for(int n = 1; n <= 5; ++n) for(int testcase = 0; testcase < 40; ++testcase) {
    soryuusi::Matrix<mint> random_matrix(n,n);
    for(auto& row : random_matrix) for(auto& x : row) {
      adjugate_seed = adjugate_seed*1103515245+12345;
      x = int((adjugate_seed>>16)%7)-3;
    }
    assert(random_matrix.adjugate() == naive_adjugate(random_matrix));
  }

  soryuusi::Matrix<long long> integer{{1,2,3},{0,4,5},{1,0,6}};
  assert(integer.det() == 22);
  assert((soryuusi::Matrix<long long>{{0,1},{1,0}}.det() == -1));
  assert((soryuusi::Matrix<long long>{{1,2},{2,4}}.det() == 0));
  assert((soryuusi::Matrix<long long>{{1,1},{1,0}}.pow(10)[0][1] == 55));
  unsigned seed = 123456789;
  for(int n = 0; n <= 5; ++n) for(int testcase = 0; testcase < 40; ++testcase) {
    soryuusi::Matrix<long long> random_matrix(n,n);
    for(auto& row : random_matrix) for(auto& x : row) {
      seed = seed*1103515245+12345;
      x = int((seed>>16)%7)-3;
    }
    assert(random_matrix.det() == naive_determinant(random_matrix));
    soryuusi::Matrix<int> int_matrix(n,n);
    for(int i = 0; i < n; ++i) for(int j = 0; j < n; ++j) int_matrix[i][j] = random_matrix[i][j];
    for(int mod = 2; mod <= 12; ++mod) {
      long long expected = naive_determinant(random_matrix)%mod;
      if(expected < 0) expected += mod;
      assert(soryuusi::matrix_det_arbitrary_mod(int_matrix,mod) == expected);
    }
  }

  soryuusi::Matrix<double> floating{{1e-20,1},{1,1}};
  assert(std::abs(floating.det()+1) < 1e-12);
  auto inverse = floating.inv();
  auto identity = floating*inverse;
  for(int i = 0; i < 2; ++i) for(int j = 0; j < 2; ++j) {
    assert(std::abs(identity[i][j]-(i == j)) < 1e-12);
  }

  using bit = soryuusi::GF2<1>;
  soryuusi::Matrix<bit> binary{{1,1,0},{1,0,1},{0,1,1}};
  assert(binary.det() == bit(0));
  soryuusi::Matrix<bit> regular_binary{{1,1},{1,0}};
  assert(regular_binary*regular_binary.inv() == soryuusi::Matrix<bit>::I(2));
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
