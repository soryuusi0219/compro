# compress

[索引](../README.md) / [実装](../../../library/utility/compress.hpp)

## 機能・前提条件

値列の座標圧縮。元の各要素の圧縮添字列と、ソート・重複除去した値列のpairを返す。

## インターフェース

```cpp
#include "library/utility/compress.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
pair<vector<long long>, vector<T>> compress(const vector<T>& v);
```

## 計算量

O(N log N) 時間、O(N) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/static_range_inversions_mo`](../../../verify/library_checker/data_structure/static_range_inversions_mo.cpp)
