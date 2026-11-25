# dynamic_segtree_2d

[索引](../README.md) / [実装](../../../library/data_structure/dynamic_segtree_2d.hpp)

## 機能・前提条件

座標 `[0,h)×[0,w)` の一点代入・加算と半開長方形の集約。`op` は可換モノイド演算、`e()` は単位元です。

一点代入では変更した子区間の集約を祖先へ返し、反対側の子だけを検索します。空長方形の集約は `e()`。問い合わせはノードを確保しません。

コピー禁止、move可能。移動元の木は空になり、再利用できます。`clone()` はノードを共有しない独立複製です。ノードはオブジェクトのプールが所有し、個別に解放しません。 1次元木と2次元木のノードは別プールに配置します。

## インターフェース

```cpp
#include "library/data_structure/dynamic_segtree_2d.hpp"
template<class S, auto op, auto e> struct Dynamic_Segtree2D;
Dynamic_Segtree2D(int h, int w);
void set(int i, int j, S x);
void add(int i, int j, S x); // old <- op(old,x)
S get(int i, int j);
S prod(int hl, int hr, int wl, int wr);
S all_prod();
Dynamic_Segtree2D clone() const;
std::size_t allocated_nodes() const; // 両プールの合計
```

## 計算量

高さH、幅W、更新数Q、両プールの確保総数K。set/add/prodは最悪 O(log H log W)、getは O(log H+log W)、全体集約は O(1)。空間 O(K)、K=O(Q log H log W)。cloneは O(K) 時間・追加空間。

## 依存関係

- [internal/node_pool.hpp](internal/node_pool.md)

## Verify・利用例

- [`unit/sparse_structures`](../../../verify/unit/sparse_structures.cpp)
- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/point_add_rectangle_sum_dynamic_segtree_2d](../../../verify/library_checker/data_structure/point_add_rectangle_sum_dynamic_segtree_2d.cpp)
