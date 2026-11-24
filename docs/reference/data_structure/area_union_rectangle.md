# area_union_rectangle

[索引](../README.md) / [実装](../../../library/data_structure/area_union_rectangle.hpp)

## 機能・前提条件

軸平行長方形の和集合面積。add(l,r,d,u) で長方形を登録し area() で走査する。座標と面積を保持できる型を指定する。

長方形は `[l,r) × [d,u)`。幅や高さが0の長方形は面積に寄与しない。各セグメント木のノードに、その区間全体を覆う長方形の個数と被覆長を保持する。被覆個数は区間を分割した先で更新し、祖先を下から再集約する。

## インターフェース

```cpp
#include "library/data_structure/area_union_rectangle.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct Area_Union_Rectangle;
Area_Union_Rectangle();
Area_Union_Rectangle(int q);
void add(T l, T r, T d, T u);
T area();
```

## 計算量

長方形 N 個に対し O(N log N) 時間、O(N) 空間。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)
- [segtree_ops.hpp](internal/segtree_ops.md)

## Verify・利用例

- [`unit/rectangle_queries`](../../../verify/unit/rectangle_queries.cpp)
- [`library_checker/area_of_union_of_rectangles`](../../../verify/library_checker/data_structure/area_of_union_of_rectangles.cpp)
