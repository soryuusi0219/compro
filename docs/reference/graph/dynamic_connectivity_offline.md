# dynamic_connectivity_offline

[索引](../README.md) / [実装](../../../library/graph/dynamic_connectivity_offline.hpp)

## 機能・前提条件

辺追加削除時刻を登録し時間軸セグメント木とrollback DSUで連結性を処理する。build → run(callback)。

## インターフェース

```cpp
#include "library/graph/dynamic_connectivity_offline.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct dynamic_connectivity_offline;
using edge = pair<int,int>;
dynamic_connectivity_offline(int n_, int q_);
void add_edge(int idx, int u, int v);
void erase_edge(int idx, int u, int v);
void _add(int l, int r, const edge& e);
void build();
void run(const function<void(int)> &f);
```

## 計算量

辺有効区間 K、時刻 Q に O(K log Q log V+Q) 時間、O(V+K log Q+Q) 空間。

## 依存関係

- [dsu_undo.hpp](../data_structure/dsu_undo.md)
- [static_graph.hpp](static_graph.md)

## Verify・利用例

- [`unit/csr_graph`](../../../verify/unit/csr_graph.cpp)
