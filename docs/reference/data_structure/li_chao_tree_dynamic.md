# li_chao_tree_dynamic

[索引](../README.md) / [実装](../../../library/data_structure/li_chao_tree_dynamic.hpp)

## 機能・前提条件

動的Li Chao treeによる直線・線分の最小値（比較関数で最大値）問い合わせ。座標領域 `XMIN/XMAX` と分割の最小幅 `EPS` をテンプレート引数で指定する。

## インターフェース

```cpp
#include "library/data_structure/li_chao_tree_dynamic.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class X, class Y, class Line, auto INF, X XMIN, X XMAX, X EPS, class CompareX = less<X>, class CompareY = less<Y>>
struct Dynamic_LiChaoTree;
Dynamic_LiChaoTree();
void add_Line(Line L);
void add_Segment(Line L, X l, X r);
Y get_min(X x);
template<class T, T XMIN, T XMAX, T EPS, T INF>
using DynamicLiChaoTree = Dynamic_LiChaoTree<T,T,Line<T>,INF_Line<T,INF>,XMIN,XMAX,EPS>;
```

## 計算量

深さ D=O(log((XMAX-XMIN)/EPS)) に対し、直線追加・取得 O(D)、線分追加 O(D²)。Q追加で最大 O(QD²) ノード。

## 依存関係

- [li_chao_tree_line.hpp](li_chao_tree_line.md)

## Verify・利用例

- [`library_checker/line_add_get_min_dynamic`](../../../verify/library_checker/data_structure/line_add_get_min_dynamic.cpp)
- [`library_checker/segment_add_get_min_dynamic`](../../../verify/library_checker/data_structure/segment_add_get_min_dynamic.cpp)
