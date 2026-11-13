# merge_sort_tree

[索引](../README.md) / [実装](../../../library/data_structure/merge_sort_tree.hpp)

## 機能・前提条件

静的配列の半開区間 `[l,r)` における個数・順序統計。`count_lt` はx未満、`count_leq` はx以下。`kth_smallest(l,r,k)` のkは **1始まり**（`1<=k<=r-l`）。既定の昇順Compareで利用する。

## インターフェース

```cpp
#include "library/data_structure/merge_sort_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class Key, class Compare = less<Key>>
struct MergeSortTree;
MergeSortTree() = default;
MergeSortTree(const vector<Key>& a);
void build(const vector<Key>& a);
int count_leq(int l, int r, Key x) const;
int count_lt(int l, int r, Key x) const;
Key kth_smallest(int l, int r, int k) const;
```

## 計算量

構築 O(N log N)、count O(log² N)、kth O(log³ N)、空間 O(N log N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/static_range_count_distinct_merge_sort_tree`](../../../verify/library_checker/data_structure/static_range_count_distinct_merge_sort_tree.cpp)
