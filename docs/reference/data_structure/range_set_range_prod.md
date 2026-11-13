# range_set_range_prod

[索引](../README.md) / [実装](../../../library/data_structure/range_set_range_prod.hpp)

## 機能・前提条件

同じ値の区間代入とモノイド集約。任意の結合的opと単位元eを使う。

## インターフェース

```cpp
#include "library/data_structure/range_set_range_prod.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e>
struct RangeSetRangeProd;
RangeSetRangeProd(int n_ = 0);
RangeSetRangeProd(const vector<S>& v);
void set(int p, S x);
S get(int p);
S prod(int l, int r);
S all_prod();
void range_set(int l, int r, S x);
int max_right(int l, auto g);
int min_left(int r, auto g);
```

## 計算量

構築 O(N)、区間代入・問い合わせ O(log N) を基本とし、内部全再構築時は O(N)。空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/range_set_range_composite`](../../../verify/library_checker/data_structure/range_set_range_composite.cpp)
