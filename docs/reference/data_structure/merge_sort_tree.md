# merge_sort_tree

[索引](../README.md) / [実装](../../../library/data_structure/merge_sort_tree.hpp)

## 機能・前提条件

静的配列の半開区間 `[l,r)` における個数・順序統計。キー順は `Compare` に従う。`count_lt` はxより前、`count_leq` はxと同順位のキーまで数える。`kth_smallest(l,r,k)` のkは **1始まり**（`1<=k<=r-l`）。同順位のキーはソート済みの根の先頭側の代表値を返す。

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

構築・空間は `O(N log(N+1))`。個数は `O(log(N+1))`、`kth_smallest` は異なるキーの順位数を `D` として `O(log(N+1) log(D+1))`。長い区間は根で求めたキー境界をrankで子に写す。短区間は各ブロックを直接二分探索する。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)
- [merge_sort_tree_rank.hpp](internal/merge_sort_tree_rank.md)

## Verify・利用例

- [`unit/static_rank_and_order_statistics`](../../../verify/unit/static_rank_and_order_statistics.cpp)
- [`library_checker/static_range_count_distinct_merge_sort_tree`](../../../verify/library_checker/data_structure/static_range_count_distinct_merge_sort_tree.cpp)
