# kd_tree_builder

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/kd_tree_builder.hpp)

## 機能・前提条件

通常版と遅延版のKD木で共用する構築処理。点の登録順を元の添字とし、部分集合を二分して完全二分木の形に並べる。点の座標は各軸で順序付けできる必要がある。

## インターフェース

```cpp
template<class X, class Y> class KDTreeBuilder;
explicit KDTreeBuilder(const std::vector<std::pair<X,Y>>& p);
void build(auto leaf, auto join);
```

`build` は葉で `leaf(ノード番号, 登録順の添字)`、内部ノードで両子を構築した後に `join(ノード番号)` を呼ぶ。`N > 0` では根が1、内部ノードが `[1,N)`、葉が `[N,2*N)`。空の点集合ではコールバックを呼ばない。構築中は入力の点集合を保持する。

通常は深さで分割軸を交互に選ぶ。片方の座標が一定ならもう一方の軸で整列する。登録順でXが昇順かつYが単調な場合や、片方が一定でもう一方が単調な場合は、元の順序のまま分割できる。

## 計算量

通常の入力は `nth_element` を使い、構築は平均 `O(N log(N+1))`、作業領域は `O(N)`。整列済みで分割が不要な入力の構築は `O(N)`。コールバックの計算量は別に数える。

## 依存関係

- [internal_bit.hpp](../../math/internal/internal_bit.md)

## Verify・利用例

- [`unit/rectangle_queries`](../../../../verify/unit/rectangle_queries.cpp)
- [`library_checker/point_add_rectangle_sum_kd_tree`](../../../../verify/library_checker/data_structure/point_add_rectangle_sum_kd_tree.cpp)
- [`library_checker/dynamic_point_set_rectangle_affine_rectangle_sum`](../../../../verify/library_checker/data_structure/dynamic_point_set_rectangle_affine_rectangle_sum.cpp)
