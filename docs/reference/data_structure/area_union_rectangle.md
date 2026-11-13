# area_union_rectangle

[索引](../README.md) / [実装](../../../library/data_structure/area_union_rectangle.hpp)

## 機能・前提条件

軸平行長方形の和集合面積。add(l,r,d,u) で長方形を登録し area() で走査する。座標と面積を保持できる型を指定する。

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

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/area_of_union_of_rectangles`](../../../verify/library_checker/data_structure/area_of_union_of_rectangles.cpp)
