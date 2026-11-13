# merge_sort_tree_segtree

[索引](../README.md) / [実装](../../../library/data_structure/merge_sort_tree_segtree.hpp)

## 機能・前提条件

キー固定・モノイド値を更新できるmerge sort tree。位置区間とキー範囲による集約。可換な結合を使う。

## インターフェース

```cpp
#include "library/data_structure/merge_sort_tree_segtree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class Key, class S, auto op, auto e, class Compare = less<Key>>
struct MergeSortTree_Segtree;
MergeSortTree_Segtree() = default;
MergeSortTree_Segtree(const vector<Key>& a);
MergeSortTree_Segtree(const vector<Key>& a, const vector<S>& v);
void build(const vector<Key>& a, const vector<S>& v);
void set(int i, S x);
S get(int i);
S prod(int l, int r);
S prod_interval(int l, int r, S lo, S up, bool mn_inclusive = true, bool mx_inclusive = false);
```

## 計算量

構築 O(N log N)、更新・問い合わせ O(log² N)、空間 O(N log N)。

## 依存関係

- [segtree.hpp](segtree.md)

## Verify・利用例

- [`library_checker/point_set_range_frequency_merge_sort_tree_segtree`](../../../verify/library_checker/data_structure/point_set_range_frequency_merge_sort_tree_segtree.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
