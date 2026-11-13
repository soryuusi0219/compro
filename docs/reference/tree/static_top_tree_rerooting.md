# static_top_tree_rerooting

[索引](../README.md) / [実装](../../../library/tree/static_top_tree_rerooting.hpp)

## 機能・前提条件

静的top tree上で一点変更と任意根のDP取得を行う。順逆の状態型と各compressを渡す。

## インターフェース

```cpp
#include "library/tree/static_top_tree_rerooting.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, class Srev, auto rake, auto compress00, auto compress10, auto compress11, auto e, auto erev>
struct StaticTopTreeRerooting;
StaticTopTreeRerooting();
StaticTopTreeRerooting(int n_);
StaticTopTreeRerooting(const vector<S>& v_, const vector<Srev>& vrev_);
void add_edge(int u, int v);
void build(int root);
inline int parent(int v) const;
void set_data(const vector<S>& v, const vector<Srev>& vrev);
void set(int v, S x, Srev xrev);
inline Srev get(int x) const;
```

## 計算量

構築 O(N log N)、更新・根指定取得 O(log N) を想定、空間 O(N)。演算を O(1) とする。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/point_set_tree_path_composite_sum`](../../../verify/library_checker/tree/point_set_tree_path_composite_sum.cpp)
