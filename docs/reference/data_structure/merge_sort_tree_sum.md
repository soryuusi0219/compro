# merge_sort_tree_sum

[索引](../README.md) / [実装](../../../library/data_structure/merge_sort_tree_sum.hpp)

## 機能・前提条件

固定キー・固定重みの範囲和。各ノードのソート済み列と累積和を使う。

## インターフェース

```cpp
#include "library/data_structure/merge_sort_tree_sum.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class Key, class S, class Compare = less<Key>>
struct MergeSortTree_Sum;
MergeSortTree_Sum() = default;
MergeSortTree_Sum(const vector<Key>& a, const vector<S>& v);
void build(const vector<Key>& a, const vector<S>& v);
S prod(int l, int r);
int count_leq(int l, int r, Key x) const;
int count_lt(int l, int r, Key x) const;
S sum_leq(int l, int r, Key x) const;
S sum_lt(int l, int r, Key x) const;
```

## 計算量

構築 O(N log N)、条件付き和・個数 O(log² N)、空間 O(N log N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/static_range_sum_with_upper_bound_merge_sort_tree`](../../../verify/library_checker/data_structure/static_range_sum_with_upper_bound_merge_sort_tree.cpp)
