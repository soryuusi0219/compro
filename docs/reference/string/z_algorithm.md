# z_algorithm

[索引](../README.md) / [実装](../../../library/string/z_algorithm.hpp)

## 機能・前提条件

列の各suffixと全体との最長共通prefix長。string以外にもsizeと添字比較を持つ列を渡せる。

## インターフェース

```cpp
#include "library/string/z_algorithm.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class V>
vector<int> z_algo(V s);
```

## 計算量

O(N) 時間・空間。引数は値渡しなので入力コピーも O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/z_algorithm`](../../../verify/library_checker/string/z_algorithm.cpp)
