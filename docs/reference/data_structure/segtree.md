# segtree

[索引](../README.md) / [実装](../../../library/data_structure/segtree.hpp)

## 機能・前提条件

モノイド上の一点代入・区間集約・境界探索。区間は半開区間、探索述語は単調かつ e に対して true。

## インターフェース

```cpp
#include "library/data_structure/segtree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e>
struct Segtree;
Segtree();
Segtree(int n_);
Segtree(vector<S> v);
void set(int i, S a);
void reset(int i);
S get(int i);
S prod(int l, int r);
S all_prod();
template<class F> int max_right(int l, F f) const;
template<class F> int min_left(int r, F f) const;
```

## 計算量

構築 O(N)、更新・集約・探索 O(log N)、get/all_prod O(1)、空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/vertex_set_path_composite`](../../../verify/library_checker/tree/vertex_set_path_composite.cpp)
