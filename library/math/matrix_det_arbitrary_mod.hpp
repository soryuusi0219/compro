// https://github.com/soryuusi0219/compro/blob/main/library/math/matrix_det_arbitrary_mod.hpp
#pragma once
#include <cassert>
#include <utility>

#include "library/math/matrix.hpp"

namespace soryuusi {

using namespace std;

inline int matrix_det_arbitrary_mod(Matrix<int> matrix, int mod) {
  assert(1 < mod);
  assert(matrix.height() == matrix.width());
  int n = matrix.height();
  for(auto& row : matrix) for(auto& x : row) {
    x %= mod;
    if(x < 0) x += mod;
  }
  int result = 1, swaps = 0;
  for(int i = 0; i < n; ++i) {
    int pivot = i;
    while(pivot < n && matrix[pivot][i] == 0) ++pivot;
    if(pivot == n) return 0;
    if(pivot != i) {
      swap(matrix[pivot],matrix[i]);
      swaps ^= 1;
    }
    for(int j = i+1; j < n; ++j) {
      int x = matrix[i][i], y = matrix[j][i];
      if(y == 0) continue;
      long long a0 = 1, a1 = 0, b0 = 0, b1 = 1;
      while(x != 0 && y != 0) {
        if(x < y) {
          int quotient = y/x;
          y -= quotient*x;
          b0 -= quotient*a0;
          b1 -= quotient*a1;
          b0 %= mod; if(b0 < 0) b0 += mod;
          b1 %= mod; if(b1 < 0) b1 += mod;
        } else {
          int quotient = x/y;
          x -= quotient*y;
          a0 -= quotient*b0;
          a1 -= quotient*b1;
          a0 %= mod; if(a0 < 0) a0 += mod;
          a1 %= mod; if(a1 < 0) a1 += mod;
        }
      }
      for(int k = i; k < n; ++k) {
        int upper = matrix[i][k], lower = matrix[j][k];
        matrix[i][k] = (upper*a0+lower*a1)%mod;
        matrix[j][k] = (upper*b0+lower*b1)%mod;
      }
      if(x == 0) {
        swap(matrix[i],matrix[j]);
        swaps ^= 1;
      }
    }
    result = (long long)result*matrix[i][i]%mod;
    if(result == 0) return 0;
  }
  return swaps ? mod-result : result;
}

}  // namespace soryuusi
