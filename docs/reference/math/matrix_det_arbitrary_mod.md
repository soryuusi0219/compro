# matrix_det_arbitrary_mod

[索引](../README.md) / [実装](../../../library/math/matrix_det_arbitrary_mod.hpp)

## 機能・前提条件

整数行列の行列式を合成数を含む任意modで求める。Matrix<int>と2以上のint modを受け取り入力値を正規化する。

## インターフェース

```cpp
#include "library/math/matrix_det_arbitrary_mod.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
inline int matrix_det_arbitrary_mod(Matrix<int> matrix, int mod);
```

## 計算量

O(N³ log mod) 時間、O(N²) 空間。

## 詳細な使用方法

既存の簡易referenceを統合した内容です。

matrix_det_arbitrary_mod(matrix,mod)


整数正方行列matrixの行列式を合成数を含む任意のmodで求める。

非可逆元を割らず、Euclid互除法に対応する行基本変形を使う。計算量はO(N^3 log mod)。

modは2以上のintで指定する。入力成分は関数内でmodの非負剰余へ正規化される。

## 依存関係

- [matrix.hpp](matrix.md)

## Verify・利用例

- [`library_checker/matrix_det_arbitrary_mod_matrix`](../../../verify/library_checker/linear_algebra/matrix_det_arbitrary_mod_matrix.cpp)
- [`unit/math_misc`](../../../verify/unit/math_misc.cpp)
