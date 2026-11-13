# bitmatrix

[索引](../README.md) / [実装](../../../library/math/bitmatrix.hpp)

## 機能・前提条件

F₂上の独立したビットパック行列。成分proxyと行文字列、汎用Matrixとの変換に対応する。

H×K と K×W の積は、転置＋内積版で O(KW+HW⌈K/64⌉)、Four Russians版で O(⌈K/t⌉(H+2^t)⌈W/64⌉)。実装は t≤10 の範囲で表サイズを選びます。LU分解・特性多項式は現行実装では O(N³) です。

## インターフェース

```cpp
#include "library/math/bitmatrix.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct BitMatrixEliminationResult;
struct BitMatrixLinearSystemResult;
struct BitMatrixLU;
struct BitMatrix;
using block_type = uint64_t;
using scalar_type = bool;
using elimination_result = BitMatrixEliminationResult;
using linear_system_result = BitMatrixLinearSystemResult;
struct reference;
operator bool() const;
reference& operator=(bool value);
reference& operator=(const reference& value);
reference& flip();
struct row_reference;
reference operator[](int column);
struct const_row_reference;
bool operator[](int column) const;
BitMatrix() = default;
BitMatrix(int height, int width);
BitMatrix(initializer_list<initializer_list<int>> values);
explicit BitMatrix(const vector<string>& rows);
template<typename T>
explicit BitMatrix(const Matrix<T>& matrix);
int height() const;
int width() const;
int blocks() const;
pair<int,int> hwsize() const;
void hwresize(int height, int width);
row_reference operator[](int row);
const_row_reference operator[](int row) const;
bool get(int row, int column) const;
void set(int row, int column, bool value = true);
void flip(int row, int column);
void swap_rows(int first, int second);
void row_xor(int destination, int source);
span<const block_type> row_blocks(int row) const;
void set_row(int row, string_view bits);
string row_string(int row) const;
vector<string> to_strings() const;
template<typename T>
Matrix<T> to_matrix() const;
static BitMatrix I(int n);
BitMatrix& operator+=(const BitMatrix& other);
BitMatrix& operator-=(const BitMatrix& other);
BitMatrix& operator*=(bool scalar);
BitMatrix& operator/=(bool scalar);
BitMatrix operator+(const BitMatrix& other) const;
BitMatrix operator-(const BitMatrix& other) const;
BitMatrix operator*(bool scalar) const;
BitMatrix operator/(bool scalar) const;
friend BitMatrix operator*(bool scalar, BitMatrix matrix);
BitMatrix operator-() const;
bool operator==(const BitMatrix& other) const;
bool operator!=(const BitMatrix& other) const;
BitMatrix operator*(const BitMatrix& other) const;
BitMatrix& operator*=(const BitMatrix& other);
vector<bool> operator*(const vector<bool>& values) const;
friend vector<bool> operator*(const vector<bool>& values, const BitMatrix& matrix);
BitMatrix pow(long long exponent) const;
BitMatrix transpose() const;
BitMatrix submatrix(int row_l, int row_r, int column_l, int column_r) const;
static BitMatrix horizontal_concat(const BitMatrix& left, const BitMatrix& right);
static BitMatrix vertical_concat(const BitMatrix& upper, const BitMatrix& lower);
elimination_result row_reduce(bool reduced = true);
bool gaussian_elimination(bool reduced = true);
bool Gaussian_elimination(bool reduced = true);
int rank() const;
bool det(int n = -1) const;
linear_system_result solve(const vector<bool>& b) const;
BitMatrix adjugate() const;
optional<BitMatrix> try_inv() const;
BitMatrix inv() const;
bool is_invertible() const;
BitMatrixLU lu_decompose() const;
friend vector<bool> characteristic_polynomial(BitMatrix matrix);
struct BitMatrixLU;
bool det() const;
optional<vector<bool>> try_solve(const vector<bool>& b) const;
vector<bool> solve(const vector<bool>& b) const;
inline vector<bool> characteristic_polynomial(BitMatrix matrix);
```

## 計算量

H×Wの格納 O(H⌈W/64⌉) word。Gauss消去 O(H min(H,W)⌈W/64⌉)、正方逆行列 O(N³/64+N²)。積は形状別に方式選択。

## 詳細な使用方法

既存の簡易referenceを統合した内容です。

BitMatrix


F_2上の密行列を、各行を64bit単位でパックして保持する。

成分へのアクセスはmatrix[i][j]、get(i,j)、set(i,j,value)、flip(i,j)を利用できる。


基本API


- BitMatrix(h,w), BitMatrix{{...},...}, BitMatrix(`vector<string>`): 構築

- BitMatrix(`Matrix<T>`), `to_matrix<T>`(): 汎用Matrixとの明示的な変換

- height(), width(), hwsize(), hwresize(h,w): サイズ操作

- set_row(i,string), row_string(i), to_strings(): 0/1文字列との変換

- I(n): 単位行列

- +, -, *, /, 単項-: F_2上の成分・行列・ベクトル演算

- pow(k): 非負整数k乗

- transpose(), submatrix(), horizontal_concat(), vertical_concat(): 行列の変形


行列積


小さい入力では転置した右辺とのpopcountによる内積を使う。

大きい入力では共有次元を小ブロックに分け、右辺の行のXOR組合せ表を構築する

Four Russians methodを使う。行列の形状から演算回数を推定して自動で選択する。


消去と連立一次方程式


- row_reduce(reduced=true): 自身を階段形または被約階段形に変更する。

- rank(), det(): 階数と行列式

- solve(b): Ax=bの解の有無、階数、ピボット列、特殊解、零空間基底を返す。

- try_inv(), inv(), is_invertible(): 逆行列

- adjugate(): 随伴行列

- lu_decompose(): 行交換付きLU分解

- characteristic_polynomial(matrix): 特性多項式を定数項から順に返す。


計算量


行基本変形は1行のXORをO(W/64)で行う。

階数、行列式、逆行列、連立一次方程式は通常のビットパックGauss消去を使う。

## 依存関係

- [matrix.hpp](matrix.md)

## Verify・利用例

- [`library_checker/inverse_matrix_mod_2_bitmatrix`](../../../verify/library_checker/linear_algebra/inverse_matrix_mod_2_bitmatrix.cpp)
- [`library_checker/matrix_det_mod_2_bitmatrix`](../../../verify/library_checker/linear_algebra/matrix_det_mod_2_bitmatrix.cpp)
- [`library_checker/matrix_product_mod_2_bitmatrix`](../../../verify/library_checker/linear_algebra/matrix_product_mod_2_bitmatrix.cpp)
- [`library_checker/matrix_rank_mod_2_bitmatrix`](../../../verify/library_checker/linear_algebra/matrix_rank_mod_2_bitmatrix.cpp)
- [`library_checker/system_of_linear_equations_mod_2_bitmatrix`](../../../verify/library_checker/linear_algebra/system_of_linear_equations_mod_2_bitmatrix.cpp)
- [`unit/bitmatrix`](../../../verify/unit/bitmatrix.cpp)
