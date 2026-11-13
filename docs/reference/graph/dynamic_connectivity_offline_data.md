# dynamic_connectivity_offline_data

[索引](../README.md) / [実装](../../../library/graph/dynamic_connectivity_offline_data.hpp)

## 機能・前提条件

動的連結性に成分値の加算・集約を追加したオフライン処理。時刻ごとの質問はrunのcallbackで行う。

## インターフェース

```cpp
#include "library/graph/dynamic_connectivity_offline_data.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, auto op>
struct dynamic_connectivity_offline_data;
using edge = pair<int,int>;
dynamic_connectivity_offline_data(int n_, T v, int q_);
dynamic_connectivity_offline_data(const vector<T> & v, int q_);
void add_edge(int idx, int u, int v);
void erase_edge(int idx, int u, int v);
void apply(int idx, int x, T v);
void _adde(int l, int r, const edge& e);
void _addv(int l, int x, T v);
void build();
template<class F>
void run(F&& f);
```

## 計算量

辺・値イベント K に O(K log Q log V+Q) 時間、O(V+K log Q+Q) 空間。

## 依存関係

- [dsu_undo_with_data.hpp](../data_structure/dsu_undo_with_data.md)
- [static_graph.hpp](static_graph.md)

## Verify・利用例

- [`library_checker/dynamic_graph_vertex_add_component_sum`](../../../verify/library_checker/graph/dynamic_graph_vertex_add_component_sum.cpp)
