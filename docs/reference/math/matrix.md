# matrix

[索引](../README.md) / [実装](../../../library/math/matrix.hpp)

## 機能・前提条件

体（主にmodint）の密行列。整数の行列式と浮動小数点の部分pivotにも対応する。

## インターフェース

```cpp
#include "library/math/matrix.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<typename T>
struct matrix_traits;
static bool is_zero(const T& value, const T& = T(1));
template<floating_point T>
struct matrix_traits<T>;
static bool is_zero(T value, T scale = T(1));
template<typename T>
template<typename T>
template<typename T>
bool matrix_is_zero(const T& value, const T& scale = T(1));
template<typename T>
int matrix_pivot(const vector<vector<T>>& a, int row, int column);
template<signed_integral T>
T matrix_integer_determinant(const vector<vector<T>>& a, int n);
template<typename T>
struct MatrixEliminationResult;
template<typename T>
struct MatrixLinearSystemResult;
template<typename T>
struct MatrixLU;
template<typename T>
struct Matrix : vector<vector<T>>;
using scalar_type = T;
using elimination_result = MatrixEliminationResult<T>;
using linear_system_result = MatrixLinearSystemResult<T>;
using vector<vector<T>>::vector;
Matrix(int h,int w);
int height() const;
int width() const;
pair<int,int> hwsize() const;
void hwresize(int h,int w);
static Matrix I(int n);
Matrix& operator+=(const Matrix& other);
Matrix& operator-=(const Matrix& other);
Matrix& operator*=(const T& k);
Matrix& operator/=(const T& k) requires requires(T a);
Matrix& operator%=(const T& k) requires requires(T a);
Matrix operator+(const Matrix& other) const;
Matrix operator-(const Matrix& other) const;
Matrix operator*(const T& k) const;
Matrix operator/(const T& k) const requires requires(T a);
Matrix operator%(const T& k) const requires requires(T a);
friend Matrix operator*(const T& k, Matrix matrix);
Matrix operator-() const;
bool operator==(const Matrix& other) const;
bool operator!=(const Matrix& other) const;
Matrix operator*(const Matrix& other) const;
Matrix operator*=(const Matrix& other);
vector<T> operator*(const vector<T>& values) const;
friend vector<T> operator*(const vector<T>& values, const Matrix& matrix);
Matrix pow(long long k) const;
Matrix transpose() const;
Matrix submatrix(int row_l, int row_r, int column_l, int column_r) const;
static Matrix horizontal_concat(const Matrix& left, const Matrix& right);
static Matrix vertical_concat(const Matrix& upper, const Matrix& lower);
elimination_result row_reduce(bool reduced = true) requires internal::matrix_field<T>;
T gaussian_elimination(bool reduced = true) requires internal::matrix_field<T>;
T Gaussian_elimination(bool reduced = true) requires internal::matrix_field<T>;
int rank() const requires internal::matrix_field<T>;
T det(int n = -1) const;
linear_system_result solve(const vector<T>& b) const requires internal::matrix_field<T>;
Matrix adjugate() const requires internal::matrix_field<T>;
Matrix inv() const requires internal::matrix_field<T>;
bool is_invertible() const requires internal::matrix_field<T>;
template<typename T>
struct MatrixLU;
T det() const;
optional<vector<T>> try_solve(const vector<T>& b) const;
vector<T> solve(const vector<T>& b) const;
template<typename T>
template<typename T>
template<typename T>
```

## 計算量

H×KとK×Wの積 O(HKW)、正方消去・逆・LU・随伴 O(N³)、冪 O(N³ log k)、格納 O(HW)。

## 詳細な使用方法

既存の簡易referenceを統合した内容です。

`Matrix<T>`


密行列の基本演算を提供する。主用途はmodint上の行列で、加減算、スカラー倍、行列積、

二乗法、行列式、階数、逆行列、連立一次方程式、LU分解に対応する。

T(0), T(1)と通常の四則演算を利用する。


対応する型と演算


- modintなどの体: 全演算に対応する。合成数modintでは消去中の非零元が可逆である必要がある。

- 浮動小数点型: 全演算に対応し、絶対値最大の部分ピボットを選ぶ。

- 符号付き整数型: 加減算、スカラー倍、行列積、二乗法と行列式に対応する。行列式は

    Bareiss法で求める。整数型のまま一般の除算結果を表現できない処理は提供しない。

浮動小数点のゼロ判定


`matrix_traits<T>`::is_zero(value,scale)を使用する。標準の浮動小数点型では

`numeric_limits<T>`::epsilon()*64を基準とした絶対・相対誤差を使う。

問題に合わせて異なる判定が必要な型ではmatrix_traitsを特殊化できる。


基本API


- Matrix(h,w), Matrix{{...},...}: 構築

- height(), width(), hwsize(), hwresize(h,w): サイズ操作

- I(n): 単位行列

- +, -, *, /, %, 単項-: 成分、行列、行列とベクトルの演算

- pow(k): 非負整数k乗。正方行列に限る。

- transpose(): 転置行列

- submatrix(rl,rr,cl,cr): 半開区間で指定した部分行列

- horizontal_concat(left,right), vertical_concat(upper,lower): 行列の結合


消去と連立一次方程式


- row_reduce(reduced=true): 自身を階段形または被約階段形に変更し、rank、

    pivot_columns、行交換の符号を含むピボット積determinantを返す。正方フルランクなら
    determinantは行列式に等しい。
- rank(): 体上の行列の階数

- gaussian_elimination(reduced=true): 従来形式の戻り値を持つ消去。

    従来名Gaussian_eliminationも利用できる。
- solve(b): Ax=bを解き、解の有無、階数、ピボット列、特殊解、零空間の基底を返す。


行列式と逆行列


- det(n=-1): 左上n×n部分の行列式。省略時は最大の正方な左上部分を使う。

- try_inv(): 正則なら逆行列、非正則または非正方ならnulloptを返す。

- inv(): 逆行列。正方かつ正則である必要がある。

- is_invertible(): 正方かつ正則かを返す。

- adjugate(): 随伴行列。正則、余階数1、それ以下をそれぞれO(N^3)で処理する。


LU分解


- lu_decompose(): 正方行列を行交換付きで`LU分解しMatrixLU<T>`を返す。

- MatrixLU::det(): 分解から行列式を返す。

- MatrixLU::try_solve(b): 正則なら連立一次方程式の解を返す。

- MatrixLU::solve(b): 正則行列用の連立一次方程式。


特性多項式はmatrix_characteristic_polynomial.hppに分離している。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/adjugate_matrix_matrix`](../../../verify/library_checker/linear_algebra/adjugate_matrix_matrix.cpp)
- [`library_checker/inverse_matrix_matrix`](../../../verify/library_checker/linear_algebra/inverse_matrix_matrix.cpp)
- [`library_checker/matrix_det_matrix`](../../../verify/library_checker/linear_algebra/matrix_det_matrix.cpp)
- [`library_checker/matrix_product_matrix`](../../../verify/library_checker/linear_algebra/matrix_product_matrix.cpp)
- [`library_checker/matrix_rank_matrix`](../../../verify/library_checker/linear_algebra/matrix_rank_matrix.cpp)
- [`library_checker/pow_of_matrix_matrix`](../../../verify/library_checker/linear_algebra/pow_of_matrix_matrix.cpp)
- [`library_checker/system_of_linear_equations_matrix`](../../../verify/library_checker/linear_algebra/system_of_linear_equations_matrix.cpp)
- [`unit/bitmatrix`](../../../verify/unit/bitmatrix.cpp)
- [`unit/math_misc`](../../../verify/unit/math_misc.cpp)
