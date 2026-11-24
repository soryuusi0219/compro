# segtree_ops

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/segtree_ops.hpp)

## 機能・前提条件

配列上のセグメント木で共用する境界走査と保留作用の保持。葉数を2の冪 `b` とし、根を1、葉を `[b,2*b)` に置く。区間の端点や一点の位置は葉番号で渡す。

## インターフェース

```cpp
template<class F, auto composition, auto id> class SegtreeTags;
class SegtreeBoundary;
template<class Visit> void segtree_push_point(int p, int h, Visit visit);
template<class Visit> void segtree_pull_point(int p, Visit visit);
```

- `SegtreeBoundary(l,r,h)` には非空区間 `b <= l < r <= 2*b` と高さ `h = log2(b)` を渡す。`push(visit)` は区間境界をまたぐ内部ノードを祖先から、`pull(visit)` は子孫から訪れる。両境界の共通祖先は一度だけ訪れる。点専用の関数は葉の祖先すべてを同じ順序で走査する。
- `SegtreeTags(b)` は各内部ノードの作用と保留フラグを持つ。`apply(k,f)` は既存の作用の後に `f` を合成する。`push(k,act)` は作用がある場合に子へ左、右の順で `act` を呼び、親の作用を解除する。`act` 自体が子の集約やタグを更新する。

## 計算量

祖先の走査は最悪 `O(log b)`。境界の計算とタグ1件の取得・合成は `O(1)` とし、コールバックの計算量は別に数える。タグの構築と記憶量は `O(b)`、他の処理の追加領域は `O(1)`。

## 依存関係

- [internal_bit.hpp](../../math/internal/internal_bit.md)

## Verify・利用例

- [`unit/linear_segment_trees`](../../../../verify/unit/linear_segment_trees.cpp)
- [`library_checker/range_affine_point_get_dual_segtree`](../../../../verify/library_checker/data_structure/range_affine_point_get_dual_segtree.cpp)
- [`library_checker/range_chmin_chmax_add_range_sum`](../../../../verify/library_checker/data_structure/range_chmin_chmax_add_range_sum.cpp)
- [`library_checker/range_set_range_composite`](../../../../verify/library_checker/data_structure/range_set_range_composite.cpp)
