# static_range_prod

[索引](../README.md) / [実装](../../../library/data_structure/static_range_prod.hpp)

## 機能・前提条件

静的モノイド区間積。ブロック分割とdisjoint sparse tableで可逆性を要求しない。

## インターフェース

```cpp
#include "library/data_structure/static_range_prod.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e>
struct StaticRangeProd;
StaticRangeProd(vector<S> v);
S naive_prod(int l, int r);
S prod(int l, int r);
```

## 計算量

構築 O(N log N)、prod O(1)、空間 O(N log N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/staticrmq`](../../../verify/library_checker/data_structure/staticrmq.cpp)
