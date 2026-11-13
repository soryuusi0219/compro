# auxiliary_tree

[索引](../README.md) / [実装](../../../library/tree/auxiliary_tree.hpp)

## 機能・前提条件

指定頂点とそのLCAだけから補助木を構築する。add_edge/set_up後に質問する。距離付き辺版も提供。

## インターフェース

```cpp
#include "library/tree/auxiliary_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct Auxiliary_Tree : SimpleEularTour;
Auxiliary_Tree(int n, int r = 0);
void set_up();
int up(int v, int k);
pair<vector<int>,vector<int>> auxiliary_tree_parent(vector<int> s);
tuple<vector<vector<int>>,vector<int>,int> auxiliary_tree(vector<int> s);
tuple<vector<vector<pair<int,int>>>,vector<int>,int> auxiliary_tree_dist(vector<int> s);
```

## 計算量

前処理 O(N log N) を上界とする。K頂点の補助木 O(K log K)、出力 O(K)、保持 O(N log N) を上界とする。

## 依存関係

- [simple_euler_tour.hpp](simple_euler_tour.md)

## Verify・利用例

- [`unit/recursive_tree`](../../../verify/unit/recursive_tree.cpp)
