# dijkstra

[索引](../README.md) / [実装](../../../library/graph/dijkstra.hpp)

## 機能・前提条件

整数型の `Dijkstra_sum<T>` は距離をキーとするradix heapを使う。`Dijkstra<T,op,e,infty>` の一般形と浮動小数点型の加算には二分ヒープを使う。最短経路が複数ある場合は、そのうちの1つを返す。

非負重みの最短路と経路復元。テンプレートで距離型・結合・単位元・無限大を指定できる。distanceの後にpathを呼ぶ。

## インターフェース

```cpp
#include "library/graph/dijkstra.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, auto op, auto e, auto infty>
struct Dijkstra;
Dijkstra(int n_);
Dijkstra(int n_, int m);
void add_edge(int u, int v, T cost, bool undirected = false);
vector<T> distance(int v);
vector<int> path(int v);
template<class T>
struct dijkstra_sum;
static constexpr T (*op)(T,T) = [](T x, T y)->T;
static constexpr T (*e)() = []()->T;
static constexpr T (*infty)() = []()->T;
template<typename T>
using Dijkstra_sum = Dijkstra<T,internal::dijkstra_sum<T>::op,internal::dijkstra_sum<T>::e,internal::dijkstra_sum<T>::infty>;
using Dijkstrall = Dijkstra_sum<long long>;
```

## 計算量

二分ヒープは $O((V+E)\log(E+2))$ 時間。radix heapでは、距離キーの上界を $U$ として $O(V+E\log(U+2))$ 時間。空間はいずれも $O(V+E)$。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)
- [static_graph.hpp](static_graph.md)

## Verify・利用例

- [`library_checker/shortest_path`](../../../verify/library_checker/graph/shortest_path.cpp)
- [`unit/graph_algorithms`](../../../verify/unit/graph_algorithms.cpp)
