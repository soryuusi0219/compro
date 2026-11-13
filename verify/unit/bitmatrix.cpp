#include <cassert>
#include <string>
#include <tuple>
#include <vector>

#include "library/math/bitmatrix.hpp"
#include "library/math/matrix.hpp"
#include "library/math/matrix_characteristic_polynomial.hpp"
#include "library/math/modint.hpp"

using mint = soryuusi::static_modint<2>;

soryuusi::Matrix<mint> to_matrix(const soryuusi::BitMatrix& matrix) {
  soryuusi::Matrix<mint> result(matrix.height(),matrix.width());
  for(int i = 0; i < matrix.height(); ++i) for(int j = 0; j < matrix.width(); ++j) {
    result[i][j] = matrix.get(i,j);
  }
  return result;
}

soryuusi::BitMatrix to_bitmatrix(const soryuusi::Matrix<mint>& matrix) {
  soryuusi::BitMatrix result(matrix.height(),matrix.width());
  for(int i = 0; i < matrix.height(); ++i) for(int j = 0; j < matrix.width(); ++j) {
    result.set(i,j,matrix[i][j].val());
  }
  return result;
}

std::vector<bool> to_bits(const std::vector<mint>& values) {
  std::vector<bool> result(values.size());
  for(int i = 0; i < (int)values.size(); ++i) result[i] = values[i].val();
  return result;
}

int main() {
  using soryuusi::BitMatrix;
  BitMatrix a{{1,0,1},{0,1,1}};
  assert(a.height() == 2 && a.width() == 3);
  assert(a.row_string(0) == "101");
  a[0][1] = true;
  a[0][2].flip();
  assert(a.row_string(0) == "110");
  assert((BitMatrix(std::vector<std::string>{"110","011"}) == a));
  assert(BitMatrix(a.to_matrix<mint>()) == a);
  assert((a+a == BitMatrix(2,3)));
  assert((a.transpose().to_strings() == std::vector<std::string>{"10","11","01"}));
  assert((a.submatrix(0,2,1,3).to_strings() == std::vector<std::string>{"10","11"}));
  assert((BitMatrix::horizontal_concat(a,a).row_string(0) == "110110"));
  assert((BitMatrix::vertical_concat(a,a).height() == 4));

  BitMatrix resized = a;
  resized.hwresize(3,70);
  assert(resized.height() == 3 && resized.width() == 70 && resized.get(0,0));
  resized.set(2,69);
  resized.hwresize(3,65);
  resized.hwresize(3,70);
  assert(!resized.get(2,69));

  unsigned seed = 123456789;
  auto next = [&] {
    seed = seed*1103515245+12345;
    return seed>>16;
  };
  for(int testcase = 0; testcase < 100; ++testcase) {
    int n = next()%9, m = next()%9, k = next()%9;
    BitMatrix left(n,m), right(m,k);
    for(int i = 0; i < n; ++i) for(int j = 0; j < m; ++j) left.set(i,j,next()&1);
    for(int i = 0; i < m; ++i) for(int j = 0; j < k; ++j) right.set(i,j,next()&1);
    auto product = left*right;
    if(n == 0 || m == 0) assert(product == BitMatrix(n,k));
    else assert(product == to_bitmatrix(to_matrix(left)*to_matrix(right)));
  }
  for(auto [n,m,k] : std::vector<std::tuple<int,int,int>>{{1,130,129},{8,130,129},{70,129,131}}) {
    BitMatrix left(n,m), right(m,k);
    for(int i = 0; i < n; ++i) for(int j = 0; j < m; ++j) left.set(i,j,next()&1);
    for(int i = 0; i < m; ++i) for(int j = 0; j < k; ++j) right.set(i,j,next()&1);
    assert(left*right == to_bitmatrix(to_matrix(left)*to_matrix(right)));
  }

  for(int n = 0; n <= 7; ++n) for(int testcase = 0; testcase < 50; ++testcase) {
    BitMatrix matrix(n,n);
    for(int i = 0; i < n; ++i) for(int j = 0; j < n; ++j) matrix.set(i,j,next()&1);
    auto generic = to_matrix(matrix);
    assert(matrix.rank() == generic.rank());
    assert(matrix.det() == bool(generic.det().val()));
    assert(matrix.adjugate() == to_bitmatrix(generic.adjugate()));
    assert(matrix.pow(5) == to_bitmatrix(generic.pow(5)));

    auto inverse = matrix.try_inv();
    auto generic_inverse = generic.try_inv();
    assert(inverse.has_value() == generic_inverse.has_value());
    if(inverse) assert(*inverse == to_bitmatrix(*generic_inverse) && matrix**inverse == BitMatrix::I(n));

    auto lu = matrix.lu_decompose();
    assert(lu.det() == matrix.det());
    if(lu.invertible) {
      std::vector<bool> expected(n);
      for(int i = 0; i < n; ++i) expected[i] = next()&1;
      assert(lu.solve(matrix*expected) == expected);
    }

    auto polynomial = soryuusi::characteristic_polynomial(matrix);
    assert(polynomial == to_bits(soryuusi::characteristic_polynomial(generic)));
  }

  for(int n = 0; n <= 7; ++n) for(int m = 0; m <= 7; ++m) {
    for(int testcase = 0; testcase < 30; ++testcase) {
      BitMatrix matrix(n,m);
      for(int i = 0; i < n; ++i) for(int j = 0; j < m; ++j) matrix.set(i,j,next()&1);
      std::vector<bool> b(n);
      std::vector<mint> generic_b(n);
      for(int i = 0; i < n; ++i) {
        bool value = next()&1;
        b[i] = value;
        generic_b[i] = value;
      }
      auto result = matrix.solve(b);
      if(n == 0) {
        assert(result.has_solution && result.rank == 0);
        assert(result.particular_solution == std::vector<bool>(m));
        assert((int)result.nullspace_basis.size() == m);
        continue;
      }
      auto generic_result = to_matrix(matrix).solve(generic_b);
      assert(result.has_solution == generic_result.has_solution);
      assert(result.rank == generic_result.rank);
      assert(result.pivot_columns == generic_result.pivot_columns);
      if(!result.has_solution) continue;
      assert(result.particular_solution == to_bits(generic_result.particular_solution));
      assert(result.nullspace_basis.size() == generic_result.nullspace_basis.size());
      assert(matrix*result.particular_solution == b);
      for(int i = 0; i < (int)result.nullspace_basis.size(); ++i) {
        assert(result.nullspace_basis[i] == to_bits(generic_result.nullspace_basis[i]));
        assert(matrix*result.nullspace_basis[i] == std::vector<bool>(n));
      }
    }
  }
}
