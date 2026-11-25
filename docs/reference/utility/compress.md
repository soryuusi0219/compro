# compress

[索引](../README.md) / [実装](../../../library/utility/compress.hpp)

## 機能・前提条件

値列を座標圧縮し、0始まりの順位列 `index` と昇順・重複除去済みの値列 `key` を返す。`key[index[i]]` は入力の `v[i]` と等しい。空入力は空の2列を返す。一般の要素型では `<` の順序と `!=` の同値判定を一致させる。

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

添字・キーを含む空間は $O(N)$。入力が昇順なら $O(N)$ 時間。十分長い32bit以下の整数列は、値と元の添字を1つの64bit整数に格納し、最大3回の基数ソートで $O(N+2^{11})$ 時間で圧縮する。残りの入力はソートと二分探索で $O(N\log(N+1))$ 時間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/static_range_inversions_mo`](../../../verify/library_checker/data_structure/static_range_inversions_mo.cpp)
- [`unit/utility_misc`](../../../verify/unit/utility_misc.cpp)
