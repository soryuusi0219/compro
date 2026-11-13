# static_graph

[索引](../README.md) / [実装](../../../library/graph/static_graph.hpp)

## 機能・前提条件

辺を追加してbuildするCSR隣接配列。static_graph<>は重みなし、static_graph<T>は重み付き。build後に辺を走査する。

## インターフェース

```cpp
#include "library/graph/static_graph.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct static_graph_edge;
template<>
struct static_graph_edge<void>;
template<class T = void>
struct static_graph;
using edge = static_graph_edge<T>;
using edge_iterator = typename vector<edge>::const_iterator;
struct edge_range;
edge_iterator begin() const;
edge_iterator end() const;
struct vertex_iterator;
int operator*() const;
vertex_iterator& operator++();
bool operator!=(const vertex_iterator& rhs) const;
struct vertex_range;
vertex_iterator begin() const;
vertex_iterator end() const;
static_graph(int n_ = 0);
static_graph(int n_, int m);
template<class I> requires (is_void_v<T> && is_convertible_v<I,int>);
static_graph(const vector<vector<I>>& G);
template<class I, class U> requires (!is_void_v<T> && is_convertible_v<I,int> && is_convertible_v<U,T>);
static_graph(const vector<vector<pair<I,U>>>& G);
void add_edge(int u, int v, bool undirected = false) requires is_void_v<T>;
void add_edge(int u, int v, U w, bool undirected = false);
void build();
edge_range operator[](int v) const;
edge_range edges(int v) const;
vertex_range vertices(int v) const;
int size() const;
int edge_size() const;
```

## 計算量

辺追加 償却 O(1)、build O(V+E)、隣接走査 O(deg(v))、空間 O(V+E)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/shortest_path_static_graph`](../../../verify/library_checker/graph/shortest_path_static_graph.cpp)
- [`unit/static_graph`](../../../verify/unit/static_graph.cpp)
