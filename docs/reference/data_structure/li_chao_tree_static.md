# li_chao_tree_static

[索引](../README.md) / [実装](../../../library/data_structure/li_chao_tree_static.hpp)

## 機能・前提条件

問い合わせ座標を事前登録するLi Chao tree。直線・線分追加と最小値取得。

問い合わせ座標は非空のvectorで構築時に渡します。線分の有効範囲は `[l,r)`。`get_min(x)` は登録済みの座標で使います。

## インターフェース

```cpp
#include "library/data_structure/li_chao_tree_static.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class X, class Y, class Line, auto INF, class CompareX = less<X>, class CompareY = less<Y>>
struct Static_LiChaoTree;
Static_LiChaoTree(vector<X> x);
Y get_min(X x);
void add_Line(Line L);
void add_Segment(Line L, X l, X r);
template<class T, T INF>
using StaticLiChaoTree = Static_LiChaoTree<T,T,Line<T>,INF_Line<T,INF>>;
```

## 計算量

座標数 N に対し直線追加・取得 O(log N)、線分追加 O(log² N)、空間 O(N)。

## 依存関係

- [li_chao_tree_line.hpp](li_chao_tree_line.md)

## Verify・利用例

- [`library_checker/line_add_get_min_static`](../../../verify/library_checker/data_structure/line_add_get_min_static.cpp)
- [`library_checker/segment_add_get_min_static`](../../../verify/library_checker/data_structure/segment_add_get_min_static.cpp)
