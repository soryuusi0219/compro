# directed_mst

[索引](../README.md) / [実装](../../../library/graph/directed_mst.hpp)

## 機能・前提条件

根から全頂点に到達できる有向全域木のうち、重み和が最小のものを求める。重み付き有向辺に対し、最小の入辺を選び、できた閉路を縮約する。
1頂点以上のグラフを渡す。負の重み・平行辺を扱える。自己ループは無視する。
重み型 `T` は全域木の重み和、閉路縮約時の加減算と比較を扱える型とする。

## インターフェース

```cpp
#include "library/graph/directed_mst.hpp"

soryuusi::DirectedMST<long long> graph(3);
graph.add_edge(0,1,10);
graph.add_edge(0,2,20);
graph.add_edge(1,2,3);
auto [cost,incoming] = graph.solve(0);  // 13, {-1, 0, 2}
```

- `DirectedMST<T>(int n, int edge_capacity = 0)` は空のグラフを作り、指定数の辺の領域を予約する。
- `int add_edge(int from, int to, T cost)` は辺を追加し、追加順の0始まりIDを返す。頂点も0始まり。
- `std::pair<T,std::vector<int>> solve(int root) const` は重み和と各頂点に選んだ入辺のIDを返す。根の入辺は `-1`。全域木が存在しない場合は `{T{}, {}}` を返す。根を変えて繰り返し呼べる。

## 計算量

頂点数を (V)、辺数を (E) として、`solve` は O((V+E) log(V+E+1)) 時間、O(V+E) 空間。skew heapの償却上界と、サイズ順併合する巻き戻しUnion-Findの上界を使う。

## 依存関係

- [dsu_undo.hpp](../data_structure/dsu_undo.md)
- [skew_heap.hpp](../data_structure/skew_heap.md)

## Verify・利用例

- [`library_checker/directedmst`](../../../verify/library_checker/graph/directedmst.cpp)
- [`unit/csr_graph`](../../../verify/unit/csr_graph.cpp)
- [移植元のAC提出](https://judge.yosupo.jp/submission/360414)
