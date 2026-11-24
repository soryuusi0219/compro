# merge_sort_tree_bit

[索引](../README.md) / [実装](../../../library/data_structure/merge_sort_tree_bit.hpp)

## 機能・前提条件

キー固定・重み更新可能なmerge sort tree。add で重みを加え、位置区間とキー上限で和を取得する。

## インターフェース

```cpp
#include "library/data_structure/merge_sort_tree_bit.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class Key, class S, class Compare = less<Key>>
struct MergeSortTree_BIT;
MergeSortTree_BIT() = default;
MergeSortTree_BIT(const vector<Key>& a);
MergeSortTree_BIT(const vector<Key>& a, const vector<S>& v);
void build(const vector<Key>& a, const vector<S>& v);
void add(int i, S x);
S get(int i);
S sum(int l, int r);
int count_leq(int l, int r, Key x) const;
int count_lt(int l, int r, Key x) const;
S sum_leq(int l, int r, Key x) const;
S sum_lt(int l, int r, Key x) const;
```

## 計算量

構築・空間は `O(N log(N+1))`、個数・無条件の区間和は `O(log(N+1))`、更新・条件付き和は `O(log²(N+1))`。位置区間の右端には `N` を指定でき、空区間の和は0。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)
- [merge_sort_tree_rank.hpp](internal/merge_sort_tree_rank.md)
- [bit.hpp](bit.md)

## Verify・利用例

- [`unit/static_rank_and_order_statistics`](../../../verify/unit/static_rank_and_order_statistics.cpp)
- [`library_checker/point_add_rectangle_sum_merge_sort_tree_bit`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_merge_sort_tree_bit.cpp)
