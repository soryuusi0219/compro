# li_chao_tree_dynamic

[索引](../README.md) / [実装](../../../library/data_structure/li_chao_tree_dynamic.hpp)

## 機能・前提条件

動的Li Chao木による直線・半開区間上の線分の最小値取得。比較関数により最大値にも対応します。座標領域 `[XMIN,XMAX)` と最小分割幅 EPS を型に指定します。整数座標の分割幅の下限は `max(EPS,1)` で、整数中点は加算のオーバーフローを避けて計算します。INFは有効な評価値よりも常に悪い番兵です。直線の評価値は型の範囲に収めてください。

コピー禁止、move可能。移動元の木は空になり、再利用できます。`clone()` はノードを共有しない独立複製です。ノードはオブジェクトのプールが所有し、個別に解放しません。 `merge(std::move(other))` は直線・線分の包絡の和を取り、otherの木を空にします。型が同じため定義域・比較規則も一致します。各ノードの担当区間を保って再挿入するので、線分の適用範囲も維持します。

## インターフェース

```cpp
#include "library/data_structure/li_chao_tree_dynamic.hpp"
template<class X, class Y, class Line, auto INF, X XMIN, X XMAX, X EPS,
         class CompareX = std::less<X>, class CompareY = std::less<Y>>
struct Dynamic_LiChaoTree;
Dynamic_LiChaoTree();
void add_line(Line line);
void add_segment(Line line, X l, X r);
void add_Line(Line line); // 既存名
void add_Segment(Line line, X l, X r); // 既存名
Y get_min(X x);
void merge(Dynamic_LiChaoTree&& other);
Dynamic_LiChaoTree clone() const;
std::size_t allocated_nodes() const;
// 整数座標用の既存エイリアス
// DynamicLiChaoTree<T,XMIN,XMAX,EPS,INF>
```

## 計算量

分割の最大深さをDとすると、直線追加・取得は `O(D+1)`、線分追加は `O((D+1)²)`。確保済みノード数Kに対して空間 `O(K)`、cloneは `O(K)`。受取側に根がない場合はプールを移します。それ以外のmergeは相手の到達可能ノード数Mに対して `O(M(D+1)²)` 時間・追加ノード数。後者では消費元の領域を破棄・再代入まで保持します。

## 依存関係

- [internal/node_pool.hpp](internal/node_pool.md)
- [li_chao_tree_line.hpp](li_chao_tree_line.md)

## Verify・利用例

- [`unit/sparse_structures`](../../../verify/unit/sparse_structures.cpp)
- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/line_add_get_min_dynamic](../../../verify/library_checker/data_structure/line_add_get_min_dynamic.cpp)
- [library_checker/segment_add_get_min_dynamic](../../../verify/library_checker/data_structure/segment_add_get_min_dynamic.cpp)
