// https://github.com/soryuusi0219/compro/blob/main/library/math/bitmatrix.hpp
#pragma once
#include <algorithm>
#include <bit>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "library/math/matrix.hpp"

namespace soryuusi {

using namespace std;

struct BitMatrixEliminationResult {
  int rank = 0;
  bool determinant = true;
  vector<int> pivot_columns;
};

struct BitMatrixLinearSystemResult {
  bool has_solution = false;
  int rank = 0;
  vector<int> pivot_columns;
  vector<bool> particular_solution;
  vector<vector<bool>> nullspace_basis;
};

struct BitMatrixLU;

struct BitMatrix {
  using block_type = uint64_t;
  using scalar_type = bool;
  using elimination_result = BitMatrixEliminationResult;
  using linear_system_result = BitMatrixLinearSystemResult;
  static constexpr int block_bits = 64;

  struct reference {
    block_type* block;
    block_type mask;

    operator bool() const {return *block&mask;}
    reference& operator=(bool value) {
      if(value) *block |= mask;
      else *block &= ~mask;
      return *this;
    }
    reference& operator=(const reference& value) {return *this = bool(value);}
    reference& flip() {*block ^= mask; return *this;}
  };

  struct row_reference {
    BitMatrix* matrix;
    int row;
    reference operator[](int column) {
      assert(0 <= column && column < matrix->width());
      return {matrix->row_data(row)+column/block_bits,block_type(1)<<(column%block_bits)};
    }
  };

  struct const_row_reference {
    const BitMatrix* matrix;
    int row;
    bool operator[](int column) const {return matrix->get(row,column);}
  };

 private:
  int height_ = 0, width_ = 0;
  vector<block_type> data_;

  static int blocks_for(int width) {return (width+block_bits-1)/block_bits;}

  static size_t storage_size(int height, int width) {
    assert(0 <= height && 0 <= width);
    return (size_t)height*blocks_for(width);
  }

  template<typename T>
  static bool to_bit(const T& value) {
    if constexpr(requires {value.val();}) return value.val()&1;
    else if constexpr(requires {value.x;}) return value.x&1;
    else return bool(value);
  }

  block_type* row_data(int row) {
    return data_.empty() ? data_.data() : data_.data()+(size_t)row*blocks();
  }
  const block_type* row_data(int row) const {
    return data_.empty() ? data_.data() : data_.data()+(size_t)row*blocks();
  }

  void mask_unused_bits() {
    if(width_ == 0 || width_%block_bits == 0) return;
    block_type mask = (block_type(1)<<(width_%block_bits))-1;
    for(int row = 0; row < height_; ++row) row_data(row)[blocks()-1] &= mask;
  }

  void xor_rows(int destination, int source, int first_block = 0) {
    for(int block = first_block; block < blocks(); ++block) {
      row_data(destination)[block] ^= row_data(source)[block];
    }
  }

  static unsigned extract_bits(const block_type* row, int first, int length) {
    int block = first/block_bits, offset = first%block_bits;
    block_type value = row[block]>>offset;
    if(offset+length > block_bits) value |= row[block+1]<<(block_bits-offset);
    return unsigned(value&((block_type(1)<<length)-1));
  }

  BitMatrix multiply_popcount(const BitMatrix& other) const {
    BitMatrix transposed = other.transpose();
    BitMatrix result(height_,other.width_);
    for(int i = 0; i < height_; ++i) for(int j = 0; j < other.width_; ++j) {
      unsigned parity = 0;
      for(int block = 0; block < blocks(); ++block) {
        parity ^= popcount(row_data(i)[block]&transposed.row_data(j)[block])&1;
      }
      if(parity) result.set(i,j);
    }
    return result;
  }

  BitMatrix multiply_four_russians(const BitMatrix& other, int table_bits) const {
    BitMatrix result(height_,other.width_);
    int output_blocks = other.blocks();
    for(int first = 0; first < width_; first += table_bits) {
      int length = min(table_bits,width_-first);
      int table_size = 1<<length;
      vector<block_type> table((size_t)table_size*output_blocks);
      for(int mask = 1; mask < table_size; ++mask) {
        int bit = countr_zero((unsigned)mask);
        int previous = mask^(1<<bit);
        const block_type* source = other.row_data(first+bit);
        block_type* destination = table.data()+(size_t)mask*output_blocks;
        const block_type* old = table.data()+(size_t)previous*output_blocks;
        for(int block = 0; block < output_blocks; ++block) {
          destination[block] = old[block]^source[block];
        }
      }
      for(int i = 0; i < height_; ++i) {
        unsigned mask = extract_bits(row_data(i),first,length);
        const block_type* source = table.data()+(size_t)mask*output_blocks;
        for(int block = 0; block < output_blocks; ++block) result.row_data(i)[block] ^= source[block];
      }
    }
    return result;
  }

  elimination_result eliminate(int columns, bool reduced) {
    elimination_result result;
    for(int column = 0; column < columns && result.rank < height_; ++column) {
      int pivot = result.rank;
      while(pivot < height_ && !get(pivot,column)) ++pivot;
      if(pivot == height_) continue;
      swap_ranges(row_data(pivot),row_data(pivot)+blocks(),row_data(result.rank));
      int first_block = column/block_bits;
      int first_row = reduced ? 0 : result.rank+1;
      for(int row = first_row; row < height_; ++row) {
        if(row != result.rank && get(row,column)) xor_rows(row,result.rank,first_block);
      }
      result.pivot_columns.push_back(column);
      ++result.rank;
    }
    result.determinant = result.rank == height_;
    return result;
  }

 public:
  BitMatrix() = default;
  BitMatrix(int height, int width)
      : height_(height), width_(width), data_(storage_size(height,width)) {}

  BitMatrix(initializer_list<initializer_list<int>> values)
      : BitMatrix((int)values.size(),values.size() ? (int)values.begin()->size() : 0) {
    int row = 0;
    for(const auto& source : values) {
      assert((int)source.size() == width_);
      int column = 0;
      for(int value : source) set(row,column++,value&1);
      ++row;
    }
  }

  explicit BitMatrix(const vector<string>& rows)
      : BitMatrix((int)rows.size(),rows.empty() ? 0 : (int)rows[0].size()) {
    for(int i = 0; i < height_; ++i) set_row(i,rows[i]);
  }

  template<typename T>
  explicit BitMatrix(const Matrix<T>& matrix) : BitMatrix(matrix.height(),matrix.width()) {
    for(int row = 0; row < height_; ++row) for(int column = 0; column < width_; ++column) {
      set(row,column,to_bit(matrix[row][column]));
    }
  }

  int height() const {return height_;}
  int width() const {return width_;}
  int blocks() const {return blocks_for(width_);}
  pair<int,int> hwsize() const {return {height_,width_};}

  void hwresize(int height, int width) {
    assert(0 <= height && 0 <= width);
    int old_height = height_, old_blocks = blocks();
    int new_blocks = blocks_for(width);
    vector<block_type> new_data((size_t)height*new_blocks);
    int copy_blocks = min(old_blocks,new_blocks);
    for(int row = 0; copy_blocks && row < min(height,old_height); ++row) {
      copy_n(row_data(row),copy_blocks,new_data.data()+(size_t)row*new_blocks);
    }
    data_ = move(new_data);
    height_ = height;
    width_ = width;
    mask_unused_bits();
  }

  row_reference operator[](int row) {
    assert(0 <= row && row < height_);
    return {this,row};
  }
  const_row_reference operator[](int row) const {
    assert(0 <= row && row < height_);
    return {this,row};
  }

  bool get(int row, int column) const {
    assert(0 <= row && row < height_ && 0 <= column && column < width_);
    return row_data(row)[column/block_bits]>>(column%block_bits)&1;
  }
  void set(int row, int column, bool value = true) {(*this)[row][column] = value;}
  void flip(int row, int column) {(*this)[row][column].flip();}
  void swap_rows(int first, int second) {
    assert(0 <= first && first < height_ && 0 <= second && second < height_);
    if(first == second || blocks() == 0) return;
    swap_ranges(row_data(first),row_data(first)+blocks(),row_data(second));
  }
  void row_xor(int destination, int source) {
    assert(0 <= destination && destination < height_ && 0 <= source && source < height_);
    xor_rows(destination,source);
  }

  span<const block_type> row_blocks(int row) const {
    assert(0 <= row && row < height_);
    return {row_data(row),(size_t)blocks()};
  }

  void set_row(int row, string_view bits) {
    assert(0 <= row && row < height_ && (int)bits.size() == width_);
    fill_n(row_data(row),blocks(),block_type(0));
    for(int column = 0; column < width_; ++column) {
      assert(bits[column] == '0' || bits[column] == '1');
      if(bits[column] == '1') set(row,column);
    }
  }
  string row_string(int row) const {
    assert(0 <= row && row < height_);
    string result(width_,'0');
    for(int column = 0; column < width_; ++column) if(get(row,column)) result[column] = '1';
    return result;
  }
  vector<string> to_strings() const {
    vector<string> result(height_);
    for(int row = 0; row < height_; ++row) result[row] = row_string(row);
    return result;
  }

  template<typename T>
  Matrix<T> to_matrix() const {
    Matrix<T> result(height_,width_);
    for(int row = 0; row < height_; ++row) for(int column = 0; column < width_; ++column) {
      result[row][column] = T(get(row,column));
    }
    return result;
  }

  static BitMatrix I(int n) {
    BitMatrix result(n,n);
    for(int i = 0; i < n; ++i) result.set(i,i);
    return result;
  }

  BitMatrix& operator+=(const BitMatrix& other) {
    assert(hwsize() == other.hwsize());
    for(int row = 0; row < height_; ++row) for(int block = 0; block < blocks(); ++block) {
      row_data(row)[block] ^= other.row_data(row)[block];
    }
    return *this;
  }
  BitMatrix& operator-=(const BitMatrix& other) {
    assert(hwsize() == other.hwsize());
    for(int row = 0; row < height_; ++row) for(int block = 0; block < blocks(); ++block) {
      row_data(row)[block] ^= other.row_data(row)[block];
    }
    return *this;
  }
  BitMatrix& operator*=(bool scalar) {
    if(!scalar) fill(data_.begin(),data_.end(),block_type(0));
    return *this;
  }
  BitMatrix& operator/=(bool scalar) {assert(scalar); return *this;}
  BitMatrix operator+(const BitMatrix& other) const {return BitMatrix(*this) += other;}
  BitMatrix operator-(const BitMatrix& other) const {return BitMatrix(*this) -= other;}
  BitMatrix operator*(bool scalar) const {return BitMatrix(*this) *= scalar;}
  BitMatrix operator/(bool scalar) const {return BitMatrix(*this) /= scalar;}
  friend BitMatrix operator*(bool scalar, BitMatrix matrix) {return matrix *= scalar;}
  BitMatrix operator-() const {return *this;}

  bool operator==(const BitMatrix& other) const {
    return hwsize() == other.hwsize() && data_ == other.data_;
  }
  bool operator!=(const BitMatrix& other) const {return !(*this == other);}

  BitMatrix operator*(const BitMatrix& other) const {
    assert(width_ == other.height_);
    if(height_ == 0 || width_ == 0 || other.width_ == 0) return BitMatrix(height_,other.width_);
    long long popcount_cost = (long long)height_*other.width_*blocks();
    int best_bits = 1;
    long long best_cost = (1LL<<60);
    for(int bits = 1; bits <= min(10,width_); ++bits) {
      long long cost = (long long)((width_+bits-1)/bits)*(height_+(1<<bits))*other.blocks();
      if(cost < best_cost) best_cost = cost, best_bits = bits;
    }
    return best_cost < popcount_cost ? multiply_four_russians(other,best_bits)
                                      : multiply_popcount(other);
  }
  BitMatrix& operator*=(const BitMatrix& other) {
    assert(width_ == other.height_ && other.width_ == other.height_);
    return *this = *this*other;
  }

  vector<bool> operator*(const vector<bool>& values) const {
    assert(width_ == (int)values.size());
    vector<block_type> packed(blocks());
    for(int column = 0; column < width_; ++column) if(values[column]) {
      packed[column/block_bits] |= block_type(1)<<(column%block_bits);
    }
    vector<bool> result(height_);
    for(int row = 0; row < height_; ++row) {
      unsigned parity = 0;
      for(int block = 0; block < blocks(); ++block) parity ^= popcount(row_data(row)[block]&packed[block])&1;
      result[row] = parity;
    }
    return result;
  }

  friend vector<bool> operator*(const vector<bool>& values, const BitMatrix& matrix) {
    assert((int)values.size() == matrix.height_);
    vector<bool> result(matrix.width_);
    for(int row = 0; row < matrix.height_; ++row) if(values[row]) {
      for(int column = 0; column < matrix.width_; ++column) result[column] = result[column]^matrix.get(row,column);
    }
    return result;
  }

  BitMatrix pow(long long exponent) const {
    assert(height_ == width_ && 0 <= exponent);
    BitMatrix result = I(height_), base = *this;
    while(exponent) {
      if(exponent&1) result *= base;
      base *= base;
      exponent >>= 1;
    }
    return result;
  }

  BitMatrix transpose() const {
    BitMatrix result(width_,height_);
    for(int row = 0; row < height_; ++row) for(int column = 0; column < width_; ++column) {
      if(get(row,column)) result.set(column,row);
    }
    return result;
  }

  BitMatrix submatrix(int row_l, int row_r, int column_l, int column_r) const {
    assert(0 <= row_l && row_l <= row_r && row_r <= height_);
    assert(0 <= column_l && column_l <= column_r && column_r <= width_);
    BitMatrix result(row_r-row_l,column_r-column_l);
    for(int row = row_l; row < row_r; ++row) for(int column = column_l; column < column_r; ++column) {
      if(get(row,column)) result.set(row-row_l,column-column_l);
    }
    return result;
  }

  static BitMatrix horizontal_concat(const BitMatrix& left, const BitMatrix& right) {
    assert(left.height_ == right.height_);
    BitMatrix result(left.height_,left.width_+right.width_);
    for(int row = 0; row < result.height_; ++row) {
      for(int column = 0; column < left.width_; ++column) if(left.get(row,column)) result.set(row,column);
      for(int column = 0; column < right.width_; ++column) if(right.get(row,column)) {
        result.set(row,column+left.width_);
      }
    }
    return result;
  }

  static BitMatrix vertical_concat(const BitMatrix& upper, const BitMatrix& lower) {
    assert(upper.width_ == lower.width_);
    BitMatrix result(upper.height_+lower.height_,upper.width_);
    copy(upper.data_.begin(),upper.data_.end(),result.data_.begin());
    copy(lower.data_.begin(),lower.data_.end(),result.data_.begin()+upper.data_.size());
    return result;
  }

  elimination_result row_reduce(bool reduced = true) {return eliminate(width_,reduced);}

  bool gaussian_elimination(bool reduced = true) {
    return row_reduce(reduced).rank == height_;
  }
  bool Gaussian_elimination(bool reduced = true) {return gaussian_elimination(reduced);}

  int rank() const {
    BitMatrix copy(*this);
    return copy.row_reduce(false).rank;
  }

  bool det(int n = -1) const {
    assert(-1 <= n && n <= min(height_,width_));
    if(n == -1) n = min(height_,width_);
    BitMatrix copy = submatrix(0,n,0,n);
    return copy.row_reduce(false).rank == n;
  }

  linear_system_result solve(const vector<bool>& b) const {
    assert((int)b.size() == height_);
    BitMatrix augmented(height_,width_+1);
    for(int row = 0; row < height_; ++row) {
      for(int column = 0; column < width_; ++column) if(get(row,column)) augmented.set(row,column);
      augmented.set(row,width_,b[row]);
    }
    auto elimination = augmented.eliminate(width_,true);
    linear_system_result result;
    result.rank = elimination.rank;
    result.pivot_columns = elimination.pivot_columns;
    for(int row = elimination.rank; row < height_; ++row) {
      if(augmented.get(row,width_)) return result;
    }
    result.has_solution = true;
    result.particular_solution.assign(width_,false);
    vector<bool> is_pivot(width_);
    for(int row = 0; row < elimination.rank; ++row) {
      int pivot = elimination.pivot_columns[row];
      is_pivot[pivot] = true;
      result.particular_solution[pivot] = augmented.get(row,width_);
    }
    for(int free_column = 0; free_column < width_; ++free_column) {
      if(is_pivot[free_column]) continue;
      vector<bool> basis(width_);
      basis[free_column] = true;
      for(int row = 0; row < elimination.rank; ++row) {
        basis[elimination.pivot_columns[row]] = augmented.get(row,free_column);
      }
      result.nullspace_basis.push_back(move(basis));
    }
    return result;
  }

  BitMatrix adjugate() const {
    assert(height_ == width_);
    int n = height_;
    if(n == 0) return BitMatrix(0,0);
    BitMatrix augmented(n,2*n);
    for(int row = 0; row < n; ++row) {
      for(int column = 0; column < n; ++column) if(get(row,column)) augmented.set(row,column);
      augmented.set(row,row+n);
    }
    auto elimination = augmented.eliminate(n,true);
    BitMatrix result(n,n);
    if(elimination.rank == n) {
      for(int row = 0; row < n; ++row) for(int column = 0; column < n; ++column) {
        result.set(row,column,augmented.get(row,column+n));
      }
      return result;
    }
    if(elimination.rank+1 < n) return result;

    vector<bool> is_pivot(n), right_kernel(n), left_kernel(n);
    for(int column : elimination.pivot_columns) is_pivot[column] = true;
    int free_column = 0;
    while(is_pivot[free_column]) ++free_column;
    right_kernel[free_column] = true;
    for(int row = 0; row < elimination.rank; ++row) {
      right_kernel[elimination.pivot_columns[row]] = augmented.get(row,free_column);
    }
    for(int column = 0; column < n; ++column) {
      left_kernel[column] = augmented.get(elimination.rank,column+n);
    }
    int row = 0, column = 0;
    while(!right_kernel[row]) ++row;
    while(!left_kernel[column]) ++column;
    bool cofactor = true;
    if(n > 1) {
      BitMatrix minor(n-1,n-1);
      for(int i = 0, ii = 0; i < n; ++i) {
        if(i == column) continue;
        for(int j = 0, jj = 0; j < n; ++j) {
          if(j == row) continue;
          if(get(i,j)) minor.set(ii,jj);
          ++jj;
        }
        ++ii;
      }
      cofactor = minor.det();
    }
    if(cofactor) for(int i = 0; i < n; ++i) if(right_kernel[i]) {
      for(int j = 0; j < n; ++j) if(left_kernel[j]) result.set(i,j);
    }
    return result;
  }

  optional<BitMatrix> try_inv() const {
    if(height_ != width_) return nullopt;
    int n = height_;
    BitMatrix augmented(n,2*n);
    for(int row = 0; row < n; ++row) {
      for(int column = 0; column < n; ++column) if(get(row,column)) augmented.set(row,column);
      augmented.set(row,row+n);
    }
    if(augmented.eliminate(n,true).rank != n) return nullopt;
    BitMatrix result(n,n);
    for(int row = 0; row < n; ++row) for(int column = 0; column < n; ++column) {
      result.set(row,column,augmented.get(row,column+n));
    }
    return result;
  }

  BitMatrix inv() const {
    auto result = try_inv();
    assert(result.has_value());
    return move(*result);
  }

  bool is_invertible() const {return height_ == width_ && rank() == height_;}
  BitMatrixLU lu_decompose() const;

  friend vector<bool> characteristic_polynomial(BitMatrix matrix);
  friend struct BitMatrixLU;
};

struct BitMatrixLU {
  BitMatrix matrix;
  vector<int> permutation;
  int swap_count = 0;
  bool invertible = false;

  bool det() const {return invertible;}

  optional<vector<bool>> try_solve(const vector<bool>& b) const {
    int n = matrix.height();
    if(!invertible || (int)b.size() != n) return nullopt;
    vector<bool> x(n);
    for(int i = 0; i < n; ++i) x[i] = b[permutation[i]];
    for(int i = 0; i < n; ++i) for(int j = 0; j < i; ++j) {
      x[i] = x[i]^(matrix.get(i,j)&x[j]);
    }
    for(int i = n-1; i >= 0; --i) for(int j = i+1; j < n; ++j) {
      x[i] = x[i]^(matrix.get(i,j)&x[j]);
    }
    return x;
  }

  vector<bool> solve(const vector<bool>& b) const {
    auto result = try_solve(b);
    assert(result.has_value());
    return move(*result);
  }
};

inline BitMatrixLU BitMatrix::lu_decompose() const {
  assert(height_ == width_);
  int n = height_;
  BitMatrixLU result{*this,vector<int>(n),0,true};
  for(int i = 0; i < n; ++i) result.permutation[i] = i;
  for(int column = 0; column < n; ++column) {
    int pivot = column;
    while(pivot < n && !result.matrix.get(pivot,column)) ++pivot;
    if(pivot == n) {
      result.invertible = false;
      break;
    }
    if(pivot != column) {
      result.matrix.swap_rows(pivot,column);
      swap(result.permutation[pivot],result.permutation[column]);
      ++result.swap_count;
    }
    for(int row = column+1; row < n; ++row) if(result.matrix.get(row,column)) {
      for(int j = column+1; j < n; ++j) {
        if(result.matrix.get(column,j)) result.matrix.flip(row,j);
      }
    }
  }
  return result;
}

inline vector<bool> characteristic_polynomial(BitMatrix matrix) {
  assert(matrix.height_ == matrix.width_);
  int n = matrix.height_;
  for(int i = 0; i+2 < n; ++i) {
    int pivot = i+1;
    while(pivot < n && !matrix.get(pivot,i)) ++pivot;
    if(pivot == n) continue;
    if(pivot != i+1) {
      matrix.swap_rows(pivot,i+1);
      for(int row = 0; row < n; ++row) {
        bool first = matrix.get(row,pivot), second = matrix.get(row,i+1);
        matrix.set(row,pivot,second);
        matrix.set(row,i+1,first);
      }
    }
    for(int row = i+2; row < n; ++row) if(matrix.get(row,i)) {
      matrix.row_xor(row,i+1);
      for(int j = 0; j < n; ++j) if(matrix.get(j,row)) matrix.flip(j,i+1);
    }
  }

  vector<vector<bool>> polynomial(n+1);
  polynomial[0] = {true};
  for(int i = 1; i <= n; ++i) {
    polynomial[i].assign(i+1,false);
    for(int j = 0; j < i; ++j) {
      polynomial[i][j] = polynomial[i][j]^(polynomial[i-1][j]&matrix.get(i-1,i-1));
      polynomial[i][j+1] = polynomial[i][j+1]^polynomial[i-1][j];
    }
    bool subdiagonal_product = true;
    for(int distance = 1; distance < i; ++distance) {
      subdiagonal_product = subdiagonal_product&matrix.get(i-distance,i-distance-1);
      bool coefficient = subdiagonal_product&matrix.get(i-distance-1,i-1);
      if(!coefficient) continue;
      for(int j = 0; j < i-distance; ++j) {
        polynomial[i][j] = polynomial[i][j]^polynomial[i-distance-1][j];
      }
    }
  }
  return polynomial[n];
}

}  // namespace soryuusi
