# incremental_msf

[索引](../README.md) / [実装](../../../library/graph/incremental_msf.hpp)

## 機能・前提条件

辺を順に加えた各prefixの最小全域森をオフライン分割統治で求める。辺比較、追加、snapshot、rollback、回答のcallbackを渡す。

## インターフェース

```cpp
#include "library/graph/incremental_msf.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
void incrementalMSF(int n, const vector<pair<int,int>>& es, auto comp, auto add, auto snapshot, auto rollback, auto solve);
```

## 計算量

保守的上界 O((V+E) log³(V+E)) にcallbackコストを加える。空間 O(V+E log E) を上界とする。

## 依存関係

- [dsu_undo.hpp](../data_structure/dsu_undo.md)

## Verify・利用例

- [`library_checker/incremental_minimum_spanning_forest_incremental_msf`](../../../verify/library_checker/graph/incremental_minimum_spanning_forest_incremental_msf.cpp)
