# matrix_characteristic_polynomial

[索引](../README.md) / [実装](../../../library/math/matrix_characteristic_polynomial.hpp)

## 機能・前提条件

密行列の特性多項式 det(xI-A) を定数項から返す。体上の相似変換でHessenberg化する。

## インターフェース

```cpp
#include "library/math/matrix_characteristic_polynomial.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<internal::matrix_field T>
vector<T> characteristic_polynomial(Matrix<T> matrix);
```

## 計算量

O(N³) 時間、O(N²) 空間。

## 詳細な使用方法

既存の簡易referenceを統合した内容です。

characteristic_polynomial(matrix)


正方行列matrixの特性多項式det(xI-matrix)をO(N^3)で求める。

戻り値pは係数の昇順で、p[i]がx^iの係数となる。0次正方行列では{1}を返す。


matrixを相似変換で上Hessenberg行列へ変換してから特性多項式を構築する。

除算可能な体上で利用でき、主用途はmodintである。浮動小数点型では

`matrix_traits<T>`のゼロ判定を使用する。

## 依存関係

- [matrix.hpp](matrix.md)

## Verify・利用例

- [`library_checker/characteristic_polynomial_matrix`](../../../verify/library_checker/linear_algebra/characteristic_polynomial_matrix.cpp)
- [`unit/bitmatrix`](../../../verify/unit/bitmatrix.cpp)
- [`unit/math_misc`](../../../verify/unit/math_misc.cpp)
