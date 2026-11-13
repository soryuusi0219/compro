# heavy_light_decomposition

[索引](../README.md) / [実装](../../../library/tree/heavy_light_decomposition.hpp)

## 機能・前提条件

Heavy-Light分解。パスをO(log N)個の区間に分け、LCA/up/jumpを提供する。非可換集約では左右callbackの順序を使い分ける。

## インターフェース

```cpp
#include "library/tree/heavy_light_decomposition.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct HLdecomposition;
HLdecomposition(int n_);
void add_edge(int u, int v);
void build(int root = 0);
inline int inid(int u) const;
inline int outid(int u) const;
inline int depth(int u) const;
int lca(int u, int v) const;
int up(int u, int n) const;
int dist(int u, int v) const;
int jump(int u, int v, int k) const;
template<class F> void for_each(int u, int v, F f) const;
template<class F1, class F2> void for_each(int u, int v, F1 f1, F2 f2) const;
```

## 計算量

build O(N)、LCA/up/jump O(log N)、for_eachはO(log N)個のcallback、空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/jump_on_tree_heavy_light_decomposition`](../../../verify/library_checker/tree/jump_on_tree_heavy_light_decomposition.cpp)
- [`library_checker/lca_heavy_light_decomposition`](../../../verify/library_checker/tree/lca_heavy_light_decomposition.cpp)
- [`library_checker/vertex_add_subtree_sum`](../../../verify/library_checker/tree/vertex_add_subtree_sum.cpp)
- [`library_checker/vertex_set_path_composite`](../../../verify/library_checker/tree/vertex_set_path_composite.cpp)
