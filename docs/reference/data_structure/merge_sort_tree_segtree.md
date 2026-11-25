# merge_sort_tree_segtree

[索引](../README.md) / [実装](../../../library/data_structure/merge_sort_tree_segtree.hpp)

## 機能・前提条件

キー固定・モノイド値を更新できるmerge sort tree。位置区間とキー範囲による集約。可換な結合を使う。

`prod(l,r)` は元の位置順の積を返し、`r = N` と空区間も扱う。`prod_interval` は従来のブロック走査順で集約する。各ブロックの中では `Compare` の順に並び、同順位では元の位置が大きい要素から処理する。

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

構築・空間は `O(N log(N+1))`、更新・キー条件付き集約は `O(log²(N+1))`、無条件の `prod` は `O(log(N+1))`。長い区間では2つのキー境界を根で求め、rankで子に写す。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)
- [merge_sort_tree_rank.hpp](internal/merge_sort_tree_rank.md)
- [segtree.hpp](segtree.md)

## Verify・利用例

- [`unit/static_rank_and_order_statistics`](../../../verify/unit/static_rank_and_order_statistics.cpp)
- [`library_checker/point_set_range_frequency_merge_sort_tree_segtree`](../../../verify/library_checker/data_structure/point_set_range_frequency_merge_sort_tree_segtree.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
