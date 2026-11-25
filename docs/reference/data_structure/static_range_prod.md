# static_range_prod

[索引](../README.md) / [実装](../../../library/data_structure/static_range_prod.hpp)

## 機能・前提条件

更新のない列の区間積を求める。`op` は結合的で、`e()` は単位元。非可換な演算も添字順に結合する。固定長ブロック内の累積積と、ブロックをまたぐ disjoint sparse table を使う。

## インターフェース

```cpp
#include "library/data_structure/static_range_prod.hpp"

template<class S, auto op, auto e, int BlockSize = 8>
struct StaticRangeProd;
```

- `StaticRangeProd(std::vector<S> v)` で構築する。構築済みの `data` は読み取り専用として扱う。
- `prod(l,r)` は半開区間 `[l,r)` の積。空区間は `e()`。
- `naive_prod(l,r)` は同じ区間を走査して積を返す。
- `BlockSize` は1以上の2の冪。`1` は全要素に直接 disjoint sparse table を作る。短い区間が多い場合は既定の8、広い区間が多い場合は32や64も選べる。

## 計算量

要素数を $N$、ブロック幅を $B$、ブロック数を $M=\lceil N/B\rceil$ とする。構築時間・空間は $O(N+M\log(M+1))$。別ブロックをまたぐ積は $O(1)$、同じブロック内は $O(r-l)$ で最大 $B$ 要素を走査する。演算コストを各時間に掛ける。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [区間和のLibrary Checker verify](../../../verify/library_checker/data_structure/static_range_sum_static_range_prod.cpp)
- [非可換アフィン合成のユニットテスト](../../../verify/unit/static_range_queries.cpp)
