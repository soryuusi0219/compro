# lca_doubling

[索引](../README.md) / [実装](../../../library/tree/lca_doubling.hpp)

## 機能・前提条件

二分累乗祖先表によるLCA・距離・パス上jump。add_edge後buildする。

## インターフェース

```cpp
#include "library/tree/lca_doubling.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct lca_doubling;
lca_doubling(int n);
void add_edge(int u, int v);
void build();
int up(int u, int n) const;
int lca(int u, int v) const;
int dist(int u, int v) const;
int jump(int u, int v, int k) const;
```

## 計算量

build O(N log N)、問い合わせ O(log N)、空間 O(N log N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/jump_on_tree`](../../../verify/library_checker/tree/jump_on_tree.cpp)
- [`library_checker/lca`](../../../verify/library_checker/tree/lca.cpp)
