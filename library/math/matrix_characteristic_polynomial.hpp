// https://github.com/soryuusi0219/compro/blob/main/library/math/matrix_characteristic_polynomial.hpp
#pragma once
#include <cassert>
#include <vector>

#include "library/math/matrix.hpp"

namespace soryuusi {

using namespace std;

template<internal::matrix_field T>
vector<T> characteristic_polynomial(Matrix<T> matrix) {
  assert(matrix.height() == matrix.width());
  int n = matrix.height();
  for(int i = 0; i+2 < n; ++i) {
    int pivot = i+1;
    while(pivot < n && internal::matrix_is_zero(matrix[pivot][i])) ++pivot;
    if(pivot == n) continue;
    if(pivot != i+1) {
      swap(matrix[pivot],matrix[i+1]);
      for(int j = 0; j < n; ++j) swap(matrix[j][pivot],matrix[j][i+1]);
    }
    T inverse = T(1)/matrix[i+1][i];
    for(int j = i+2; j < n; ++j) {
      if(internal::matrix_is_zero(matrix[j][i],matrix[i+1][i])) continue;
      T coefficient = matrix[j][i]*inverse;
      for(int k = i; k < n; ++k) matrix[j][k] -= coefficient*matrix[i+1][k];
      for(int k = 0; k < n; ++k) matrix[k][i+1] += coefficient*matrix[k][j];
      matrix[j][i] = T(0);
    }
  }

  vector<vector<T>> polynomial(n+1);
  polynomial[0] = {T(1)};
  for(int i = 1; i <= n; ++i) {
    polynomial[i].assign(i+1,T(0));
    for(int j = 0; j < i; ++j) {
      polynomial[i][j] -= polynomial[i-1][j]*matrix[i-1][i-1];
      polynomial[i][j+1] += polynomial[i-1][j];
    }
    T subdiagonal_product = T(1);
    for(int distance = 1; distance < i; ++distance) {
      subdiagonal_product *= matrix[i-distance][i-distance-1];
      T coefficient = subdiagonal_product*matrix[i-distance-1][i-1];
      if(internal::matrix_is_zero(coefficient)) continue;
      for(int j = 0; j < i-distance; ++j) {
        polynomial[i][j] -= coefficient*polynomial[i-distance-1][j];
      }
    }
  }
  return polynomial[n];
}

}  // namespace soryuusi
