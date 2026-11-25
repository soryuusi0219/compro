# merge_sort_tree_rank

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/merge_sort_tree_rank.hpp)

## 機能・前提条件

Merge Sort Treeで親のソート済みキー区間を子に写す内部実装。マージした要素ごとに右の子から来たかを1ビットで記録し、rankで左右の子の端点を求める。階層のキー配列は根から葉の順で、各行には位置順に全ノードのキーを詰める。

## インターフェース

```cpp
class MergeSortTreeRank;
void build(int n, int height);
void set_right(int level, int i);
void finish(int level);
void visit(int n, int l, int r, auto bounds, auto take) const;
```

- `level=0` は葉どうしをマージする階層。親の行で右の子からコピーした添字を `set_right` で記録し、行が完成した時点で `finish` を呼ぶ。
- `visit` は位置区間 `[l,r)` を標準的なセグメント木のブロックに分ける。`bounds(a,b)` はソート済みの `[a,b)` 内でキー条件を満たす半開区間を返す。長さ512以下の位置区間では各ブロックで直接境界を求め、それより長ければ根で一度求めて子へ写す。
- `take(a,b)` は各ブロックの該当部分に対して、下位階層から左端・右端の順で呼ばれる。

## 計算量

高さを `H` とすると追加の空間は `O(H(N/64+1))` ワード。長い位置区間の走査は `O(H+1)` で、これに根での境界探索と各ブロックのコールバックの時間が加わる。

## 依存関係

- [internal_bit.hpp](../../math/internal/internal_bit.md)
- [static_bit_vector.hpp](../static_bit_vector.md)

## Verify・利用例

- [`unit/static_rank_and_order_statistics`](../../../../verify/unit/static_rank_and_order_statistics.cpp)
- [`library_checker/static_range_sum_with_upper_bound_merge_sort_tree`](../../../../verify/library_checker/data_structure/static_range_sum_with_upper_bound_merge_sort_tree.cpp)
- [`library_checker/point_add_rectangle_sum_merge_sort_tree_bit`](../../../../verify/library_checker/data_structure/point_add_rectangle_sum_merge_sort_tree_bit.cpp)
