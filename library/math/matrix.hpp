// https://github.com/soryuusi0219/compro/blob/main/library/math/matrix.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <concepts>
#include <limits>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<typename T>
struct matrix_traits {
  static bool is_zero(const T& value, const T& = T(1)) {return value == T(0);}
};

template<floating_point T>
struct matrix_traits<T> {
  static constexpr T tolerance = numeric_limits<T>::epsilon()*T(64);
  static bool is_zero(T value, T scale = T(1)) {
    return abs(value) <= tolerance*max(T(1),abs(scale));
  }
};

namespace internal {

template<typename T>
concept matrix_field = !integral<T> && requires(T a, T b) {
  T(0);
  T(1);
  { a == b } -> convertible_to<bool>;
  { a != b } -> convertible_to<bool>;
  { -a } -> convertible_to<T>;
  { a += b } -> same_as<T&>;
  { a -= b } -> same_as<T&>;
  { a *= b } -> same_as<T&>;
  { a /= b } -> same_as<T&>;
};

template<typename T>
concept matrix_modint32 = requires(T value) {
  T::mod();
  value.val();
  requires integral<remove_cvref_t<decltype(value.val())>>;
  requires sizeof(value.val()) <= sizeof(unsigned int);
};

template<typename T>
bool matrix_is_zero(const T& value, const T& scale = T(1)) {
  return matrix_traits<T>::is_zero(value,scale);
}

template<typename T>
int matrix_pivot(const vector<vector<T>>& a, int row, int column) {
  if constexpr(floating_point<T>) {
    int pivot = -1;
    for(int i = row; i < (int)a.size(); ++i) {
      if(pivot == -1 || abs(a[pivot][column]) < abs(a[i][column])) pivot = i;
    }
    if(pivot == -1 || matrix_is_zero(a[pivot][column],a[pivot][column])) return -1;
    return pivot;
  } else {
    for(int i = row; i < (int)a.size(); ++i) {
      if(!matrix_is_zero(a[i][column])) return i;
    }
    return -1;
  }
}

template<signed_integral T>
T matrix_integer_determinant(const vector<vector<T>>& a, int n) {
  if(n == 0) return T(1);
  using W = conditional_t<(sizeof(T) < sizeof(long long)),long long,__int128_t>;
  vector<vector<W>> b(n,vector<W>(n));
  for(int i = 0; i < n; ++i) for(int j = 0; j < n; ++j) b[i][j] = a[i][j];
  W sign = 1, divisor = 1;
  for(int k = 0; k+1 < n; ++k) {
    int pivot = k;
    while(pivot < n && b[pivot][k] == 0) ++pivot;
    if(pivot == n) return T(0);
    if(pivot != k) {
      swap(b[pivot],b[k]);
      sign = -sign;
    }
    for(int i = k+1; i < n; ++i) {
      for(int j = k+1; j < n; ++j) {
        b[i][j] = (b[i][j]*b[k][k]-b[i][k]*b[k][j])/divisor;
      }
      b[i][k] = 0;
    }
    divisor = b[k][k];
  }
  return T(sign*b[n-1][n-1]);
}

}  // namespace internal

template<typename T>
struct MatrixEliminationResult {
  int rank = 0;
  T determinant = T(1);
  vector<int> pivot_columns;
};

template<typename T>
struct MatrixLinearSystemResult {
  bool has_solution = false;
  int rank = 0;
  vector<int> pivot_columns;
  vector<T> particular_solution;
  vector<vector<T>> nullspace_basis;
};

template<typename T>
struct MatrixLU;

// This generic implementation deliberately remains the primary Matrix<T>
// template. A specialized storage implementation can replace it without
// changing the public Matrix<T> call sites.
template<typename T>
struct Matrix : vector<vector<T>>{
  using scalar_type = T;
  using elimination_result = MatrixEliminationResult<T>;
  using linear_system_result = MatrixLinearSystemResult<T>;
  using vector<vector<T>>::vector;

  Matrix(int h,int w) : vector<vector<T>>(h,vector<T>(w)) {}

  int height() const {return (int)(*this).size();}
  int width() const {return (*this).empty() ? 0 : (int)(*this)[0].size();}
  pair<int,int> hwsize() const {return {height(),width()};}
  void hwresize(int h,int w) {
    (*this).resize(h);
    for(auto& row : *this) row.resize(w);
  }

  static Matrix I(int n){
    Matrix ret(n,n);
    for(int i = 0; i < n; ++i) ret[i][i] = T(1);
    return ret;
  }

  Matrix& operator+=(const Matrix& other) {
    assert(hwsize() == other.hwsize());
    for(int i = 0; i < height(); ++i) for(int j = 0; j < width(); ++j) (*this)[i][j] += other[i][j];
    return *this;
  }
  Matrix& operator-=(const Matrix& other) {
    assert(hwsize() == other.hwsize());
    for(int i = 0; i < height(); ++i) for(int j = 0; j < width(); ++j) (*this)[i][j] -= other[i][j];
    return *this;
  }
  Matrix& operator*=(const T& k) {
    for(auto& row : *this) for(auto& x : row) x *= k;
    return *this;
  }
  Matrix& operator/=(const T& k) requires requires(T a) {a /= a;} {
    for(auto& row : *this) for(auto& x : row) x /= k;
    return *this;
  }
  Matrix& operator%=(const T& k) requires requires(T a) {a %= a;} {
    for(auto& row : *this) for(auto& x : row) x %= k;
    return *this;
  }
  Matrix operator+(const Matrix& other) const {return Matrix(*this) += other;}
  Matrix operator-(const Matrix& other) const {return Matrix(*this) -= other;}
  Matrix operator*(const T& k) const {return Matrix(*this) *= k;}
  Matrix operator/(const T& k) const requires requires(T a) {a /= a;} {return Matrix(*this) /= k;}
  Matrix operator%(const T& k) const requires requires(T a) {a %= a;} {return Matrix(*this) %= k;}
  friend Matrix operator*(const T& k, Matrix matrix) {return matrix *= k;}

  Matrix operator-() const {
    Matrix ret(*this);
    for(auto& row : ret) for(auto& x : row) x = -x;
    return ret;
  }

  bool operator==(const Matrix& other) const {
    assert(hwsize() == other.hwsize());
    return static_cast<const vector<vector<T>>&>(*this) == static_cast<const vector<vector<T>>&>(other);
  }
  bool operator!=(const Matrix& other) const {return !(*this == other);}

  Matrix operator*(const Matrix& other) const {
    assert(width() == other.height());
    Matrix ret(height(),other.width());
    if constexpr(internal::matrix_modint32<T>) {
      vector<vector<unsigned int>> transposed(other.width(),vector<unsigned int>(width()));
      for(int i = 0; i < other.height(); ++i) for(int j = 0; j < other.width(); ++j) {
        transposed[j][i] = other[i][j].val();
      }
      for(int i = 0; i < height(); ++i) for(int j = 0; j < other.width(); ++j) {
        __uint128_t sum = 0;
        for(int k = 0; k < width(); ++k) {
          sum += (unsigned long long)(*this)[i][k].val()*transposed[j][k];
        }
        ret[i][j] = T((unsigned long long)(sum%(unsigned long long)T::mod()));
      }
    } else {
      for(int i = 0; i < height(); ++i) for(int j = 0; j < width(); ++j) {
        for(int k = 0; k < other.width(); ++k) ret[i][k] += (*this)[i][j]*other[j][k];
      }
    }
    return ret;
  }
  Matrix operator*=(const Matrix& other){
    assert(width() == other.height());
    assert(other.width() == other.height());
    return *this = *this*other;
  }
  vector<T> operator*(const vector<T>& values) const {
    assert(width() == (int)values.size());
    vector<T> ret(height());
    for(int i = 0; i < height(); ++i) for(int j = 0; j < width(); ++j) {
      ret[i] += (*this)[i][j]*values[j];
    }
    return ret;
  }
  friend vector<T> operator*(const vector<T>& values, const Matrix& matrix) {
    assert((int)values.size() == matrix.height());
    vector<T> ret(matrix.width());
    for(int i = 0; i < matrix.height(); ++i) for(int j = 0; j < matrix.width(); ++j) {
      ret[j] += values[i]*matrix[i][j];
    }
    return ret;
  }

  Matrix pow(long long k) const {
    assert(height() == width());
    assert(0 <= k);
    Matrix ret = I(height()), base = *this;
    while(k) {
      if(k&1) ret *= base;
      base *= base;
      k >>= 1;
    }
    return ret;
  }

  Matrix transpose() const {
    Matrix ret(width(),height());
    for(int i = 0; i < height(); ++i) for(int j = 0; j < width(); ++j) ret[j][i] = (*this)[i][j];
    return ret;
  }

  Matrix submatrix(int row_l, int row_r, int column_l, int column_r) const {
    assert(0 <= row_l && row_l <= row_r && row_r <= height());
    assert(0 <= column_l && column_l <= column_r && column_r <= width());
    Matrix ret(row_r-row_l,column_r-column_l);
    for(int i = row_l; i < row_r; ++i) for(int j = column_l; j < column_r; ++j) {
      ret[i-row_l][j-column_l] = (*this)[i][j];
    }
    return ret;
  }

  static Matrix horizontal_concat(const Matrix& left, const Matrix& right) {
    assert(left.height() == right.height());
    Matrix ret(left.height(),left.width()+right.width());
    for(int i = 0; i < ret.height(); ++i) {
      copy(left[i].begin(),left[i].end(),ret[i].begin());
      copy(right[i].begin(),right[i].end(),ret[i].begin()+left.width());
    }
    return ret;
  }

  static Matrix vertical_concat(const Matrix& upper, const Matrix& lower) {
    assert(upper.width() == lower.width());
    Matrix ret(upper.height()+lower.height(),upper.width());
    for(int i = 0; i < upper.height(); ++i) ret[i] = upper[i];
    for(int i = 0; i < lower.height(); ++i) ret[i+upper.height()] = lower[i];
    return ret;
  }

 private:
  elimination_result eliminate(int columns, bool reduced) requires internal::matrix_field<T> {
    elimination_result result;
    for(int column = 0; column < columns && height() > result.rank; ++column) {
      int pivot = internal::matrix_pivot(*this,result.rank,column);
      if(pivot == -1) continue;
      if(pivot != result.rank) {
        swap((*this)[pivot],(*this)[result.rank]);
        result.determinant = -result.determinant;
      }
      T pivot_value = (*this)[result.rank][column];
      result.determinant *= pivot_value;
      T inverse = T(1)/pivot_value;
      for(int j = column; j < width(); ++j) (*this)[result.rank][j] *= inverse;
      (*this)[result.rank][column] = T(1);
      for(int i = reduced ? 0 : result.rank+1; i < height(); ++i) {
        if(i == result.rank || internal::matrix_is_zero((*this)[i][column],pivot_value)) continue;
        T coefficient = (*this)[i][column];
        for(int j = column; j < width(); ++j) (*this)[i][j] -= (*this)[result.rank][j]*coefficient;
        (*this)[i][column] = T(0);
      }
      result.pivot_columns.push_back(column);
      ++result.rank;
    }
    return result;
  }

 public:
  elimination_result row_reduce(bool reduced = true) requires internal::matrix_field<T> {
    return eliminate(width(),reduced);
  }

  T gaussian_elimination(bool reduced = true) requires internal::matrix_field<T> {
    auto result = row_reduce(reduced);
    return result.rank == height() ? result.determinant : T(0);
  }

  T Gaussian_elimination(bool reduced = true) requires internal::matrix_field<T> {
    return gaussian_elimination(reduced);
  }

  int rank() const requires internal::matrix_field<T> {
    Matrix copy(*this);
    return copy.row_reduce(false).rank;
  }

  T det(int n = -1) const {
    assert(-1 <= n && n <= min(height(),width()));
    if(n == -1) n = min(height(),width());
    if constexpr(signed_integral<T>) {
      return internal::matrix_integer_determinant(*this,n);
    } else {
      static_assert(internal::matrix_field<T>,
                    "Matrix::det requires a field-like or signed integral scalar type");
      Matrix copy = submatrix(0,n,0,n);
      auto result = copy.row_reduce(false);
      return result.rank == n ? result.determinant : T(0);
    }
  }

  linear_system_result solve(const vector<T>& b) const requires internal::matrix_field<T> {
    assert((int)b.size() == height());
    Matrix augmented(height(),width()+1);
    for(int i = 0; i < height(); ++i) {
      copy((*this)[i].begin(),(*this)[i].end(),augmented[i].begin());
      augmented[i][width()] = b[i];
    }
    auto elimination = augmented.eliminate(width(),true);
    linear_system_result result;
    result.rank = elimination.rank;
    result.pivot_columns = elimination.pivot_columns;
    for(int i = elimination.rank; i < height(); ++i) {
      if(!internal::matrix_is_zero(augmented[i][width()])) return result;
    }
    result.has_solution = true;
    result.particular_solution.assign(width(),T(0));
    vector<bool> is_pivot(width());
    for(int i = 0; i < elimination.rank; ++i) {
      int pivot = elimination.pivot_columns[i];
      is_pivot[pivot] = true;
      result.particular_solution[pivot] = augmented[i][width()];
    }
    for(int free_column = 0; free_column < width(); ++free_column) {
      if(is_pivot[free_column]) continue;
      vector<T> basis(width());
      basis[free_column] = T(1);
      for(int i = 0; i < elimination.rank; ++i) {
        basis[elimination.pivot_columns[i]] = -augmented[i][free_column];
      }
      result.nullspace_basis.push_back(move(basis));
    }
    return result;
  }

  Matrix adjugate() const requires internal::matrix_field<T> {
    assert(height() == width());
    int n = height();
    if(n == 0) return Matrix(0,0);
    Matrix augmented(n,2*n);
    for(int i = 0; i < n; ++i) {
      copy((*this)[i].begin(),(*this)[i].end(),augmented[i].begin());
      augmented[i][i+n] = T(1);
    }
    auto elimination = augmented.eliminate(n,true);
    Matrix result(n,n);
    if(elimination.rank == n) {
      for(int i = 0; i < n; ++i) for(int j = 0; j < n; ++j) {
        result[i][j] = elimination.determinant*augmented[i][j+n];
      }
      return result;
    }
    if(elimination.rank+1 < n) return result;

    vector<bool> is_pivot(n);
    for(int column : elimination.pivot_columns) is_pivot[column] = true;
    int free_column = 0;
    while(is_pivot[free_column]) ++free_column;
    vector<T> right_kernel(n), left_kernel(n);
    right_kernel[free_column] = T(1);
    for(int i = 0; i < elimination.rank; ++i) {
      right_kernel[elimination.pivot_columns[i]] = -augmented[i][free_column];
    }
    for(int j = 0; j < n; ++j) left_kernel[j] = augmented[elimination.rank][j+n];
    int row = 0, column = 0;
    while(internal::matrix_is_zero(right_kernel[row])) ++row;
    while(internal::matrix_is_zero(left_kernel[column])) ++column;

    Matrix minor(n-1,n-1);
    for(int i = 0, ii = 0; i < n; ++i) {
      if(i == column) continue;
      for(int j = 0, jj = 0; j < n; ++j) {
        if(j == row) continue;
        minor[ii][jj++] = (*this)[i][j];
      }
      ++ii;
    }
    T cofactor = minor.det();
    if((row+column)&1) cofactor = -cofactor;
    T scale = cofactor/(right_kernel[row]*left_kernel[column]);
    for(int i = 0; i < n; ++i) for(int j = 0; j < n; ++j) {
      result[i][j] = scale*right_kernel[i]*left_kernel[j];
    }
    return result;
  }

  auto try_inv() const requires internal::matrix_field<T>;
  Matrix inv() const requires internal::matrix_field<T>;
  bool is_invertible() const requires internal::matrix_field<T> {
    return height() == width() && rank() == height();
  }
  auto lu_decompose() const requires internal::matrix_field<T>;
};

template<typename T>
struct MatrixLU {
  Matrix<T> matrix;
  vector<int> permutation;
  int swap_count = 0;
  bool invertible = false;

  T det() const {
    if(!invertible) return T(0);
    T result = swap_count&1 ? T(-1) : T(1);
    for(int i = 0; i < matrix.height(); ++i) result *= matrix[i][i];
    return result;
  }

  optional<vector<T>> try_solve(const vector<T>& b) const {
    int n = matrix.height();
    if(!invertible || (int)b.size() != n) return nullopt;
    vector<T> x(n);
    for(int i = 0; i < n; ++i) x[i] = b[permutation[i]];
    for(int i = 0; i < n; ++i) for(int j = 0; j < i; ++j) x[i] -= matrix[i][j]*x[j];
    for(int i = n-1; i >= 0; --i) {
      for(int j = i+1; j < n; ++j) x[i] -= matrix[i][j]*x[j];
      x[i] /= matrix[i][i];
    }
    return x;
  }

  vector<T> solve(const vector<T>& b) const {
    auto result = try_solve(b);
    assert(result.has_value());
    return move(*result);
  }
};

template<typename T>
auto Matrix<T>::try_inv() const requires internal::matrix_field<T> {
  if(height() != width()) return optional<Matrix>{};
  int n = height();
  Matrix augmented(n,2*n);
  for(int i = 0; i < n; ++i) for(int j = 0; j < n; ++j) augmented[i][j] = (*this)[i][j];
  for(int i = 0; i < n; ++i) augmented[i][i+n] = T(1);
  if(augmented.eliminate(n,true).rank != n) return optional<Matrix>{};
  Matrix result(n,n);
  for(int i = 0; i < n; ++i) for(int j = 0; j < n; ++j) result[i][j] = augmented[i][j+n];
  return optional<Matrix>{move(result)};
}

template<typename T>
Matrix<T> Matrix<T>::inv() const requires internal::matrix_field<T> {
  auto result = try_inv();
  assert(result.has_value());
  return move(*result);
}

template<typename T>
auto Matrix<T>::lu_decompose() const requires internal::matrix_field<T> {
  assert(height() == width());
  int n = height();
  MatrixLU<T> result{*this,vector<int>(n),0,true};
  for(int i = 0; i < n; ++i) result.permutation[i] = i;
  for(int k = 0; k < n; ++k) {
    int pivot = internal::matrix_pivot(result.matrix,k,k);
    if(pivot == -1) {
      result.invertible = false;
      break;
    }
    if(pivot != k) {
      swap(result.matrix[pivot],result.matrix[k]);
      swap(result.permutation[pivot],result.permutation[k]);
      ++result.swap_count;
    }
    for(int i = k+1; i < n; ++i) {
      result.matrix[i][k] /= result.matrix[k][k];
      T coefficient = result.matrix[i][k];
      for(int j = k+1; j < n; ++j) result.matrix[i][j] -= coefficient*result.matrix[k][j];
    }
  }
  return result;
}

}  // namespace soryuusi
