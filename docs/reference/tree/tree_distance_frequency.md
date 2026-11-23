# tree_distance_frequency

[索引](../README.md) / [実装](../../../library/tree/tree_distance_frequency.hpp)

## 機能・前提条件

連結な無向木について、距離がちょうど (d) の異なる頂点の非順序対を、すべての (d) で数える。距離は辺数。1頂点以上の木を渡す。
重心分解で頂点対を分け、深さの頻度表を畳み込む。小さい頻度表は直接計算し、係数の上界が998244353未満ならその法で1回だけ畳み込む。大きい係数には2つのNTT素数とCRTを使う。畳み込みの変換長は $2^{23}$ 以下の範囲で使う。

## インターフェース

```cpp
#include "library/tree/tree_distance_frequency.hpp"

soryuusi::TreeDistanceFrequency tree(3);
tree.add_edge(0,1);
tree.add_edge(1,2);
auto counts = tree.solve();  // {0, 2, 1}
```

- `TreeDistanceFrequency(int n)` で (n) 頂点を用意し、`void add_edge(int u, int v)` で最初の `solve()` までに無向辺を (n-1) 本追加する。頂点は0始まり。
- `std::vector<long long> solve()` は長さ (n) の頻度表を返す。`counts[0]` は0。2回目以降は重心分解を再利用する。

## 計算量

頂点数 (N) に対し最悪 O(N log²(N+1)) 時間、O(N) 空間。

## 依存関係

- [centroid_decomposition.hpp](centroid_decomposition.md)
- [convolution.hpp](../math/convolution.md)
- [internal_math.hpp](../math/internal/internal_math.md)
- [modint.hpp](../math/modint.md)

## Verify・利用例

- [`library_checker/frequency_table_of_tree_distance`](../../../verify/library_checker/tree/frequency_table_of_tree_distance.cpp)
- [`unit/tree_algorithms`](../../../verify/unit/tree_algorithms.cpp)
