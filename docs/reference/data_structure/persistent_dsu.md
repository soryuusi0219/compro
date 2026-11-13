# persistent_dsu

[索引](../README.md) / [実装](../../../library/data_structure/persistent_dsu.hpp)

## 機能・前提条件

rootを引数に取る永続Union-Find。init で初期版を作り merge の返す新rootを保存する。

## インターフェース

```cpp
#include "library/data_structure/persistent_dsu.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct Persistent_dsu;
using PV=Persistent_Vector<pair<int, int>>;
using np = PV::np;
Persistent_dsu();
np init(int m);
int leader(int i,np& t);
bool same(int i,int j,np& t);
pair<int, np> merge(int i,int j,np& t);
int size(int i, np& t);
using Pdsu = Persistent_dsu;
using np = Pdsu::np;
```

## 計算量

代表探索・併合 O(log² N) を上界とする。更新ごとの永続配列経路は O(log N) ノード。

## 依存関係

- [persistent_vector.hpp](persistent_vector.md)

## Verify・利用例

- [`library_checker/persistent_unionfind`](../../../verify/library_checker/data_structure/persistent_unionfind.cpp)
