# static_range_inversions

[索引](../README.md) / [実装](../../../library/data_structure/static_range_inversions.hpp)

## 機能・前提条件

静的配列の半開区間 `[l,r)` の転倒数。Compareが `a[j] < a[i]` と判定する `l <= i < j < r` を数えます。比較で等価な要素間には転倒がありません。構築時のqは想定クエリ数で、省略時は配列長、0なら作業表を使う場合に全体を1ブロックにします。

## インターフェース

```cpp
#include "library/data_structure/static_range_inversions.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class Compare = less<T>>
struct StaticRangeInversions;
using ll = long long;
StaticRangeInversions(const vector<T>& v, int q = -1);
long long query(int x, int y);
```

## 計算量

ブロック長Lに対し構築は O(N log(N+1)+N²/L)、queryは最悪 O(L)、空間は O(N²/L)。既定Lは概ね√Nです。短区間は直接数え、境界から始まる問い合わせは集計表を使います。入力がCompare順で単調非減少または狭義単調減少の場合、構築は O(N)、queryと追加領域は O(1) です。

隣の境界との差分が32bitに収まる場合、集計表を32bitで保持し、必要ならブロック境界間の64bit集計から復元します。差分に64bitが必要な場合は表も64bitにします。戻り値はいずれも64bitです。

## 依存関係

- [bit.hpp](bit.md)

## Verify・利用例

- [`library_checker/static_range_inversions`](../../../verify/library_checker/data_structure/static_range_inversions.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
