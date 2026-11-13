# determinant_sparse

[索引](../README.md) / [実装](../../../library/math/determinant_sparse.hpp)

## 機能・前提条件

疎行列を (行,列,値) のtuple列で与えて行列式を求める。乱択と線形漸化式を使う確率的アルゴリズム。

## インターフェース

```cpp
#include "library/math/determinant_sparse.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
T determinant_sparse(int n, vector<tuple<int,int,T>> val);
```

## 計算量

非零数 E、サイズN に O(N(E+N)) 時間、O(E+N) 空間を上界とする。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/sparse_matrix_det`](../../../verify/library_checker/linear_algebra/sparse_matrix_det.cpp)
