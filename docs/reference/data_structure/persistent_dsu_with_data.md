# persistent_dsu_with_data

[索引](../README.md) / [実装](../../../library/data_structure/persistent_dsu_with_data.hpp)

## 機能・前提条件

成分集約付き永続Union-Find。通常版と同じ Persistent_dsu という名前なので同時includeを避ける。

## インターフェース

```cpp
#include "library/data_structure/persistent_dsu_with_data.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op>
struct Persistent_dsu;
struct Persistent_Vector;
struct node;
using np=node*;
struct node;
Persistent_Vector();
np get_root();
void des_set(int id, int val, S s, int sz, np& t);
np set(int id, int val, S s, int sz, np& t);
np mutable_get(int id, np t);
tuple<long long,S,long long> get(int id, np t);
using PV=Persistent_Vector;
using np = PV::np;
Persistent_dsu();
np init(int m, S s);
int leader(int i,np& t);
bool same(int i,int j,np& t);
pair<long long,np> merge(int i,int j,np& t);
S get(int i, np& t);
int size(int i, np& t);
np set(int i, S s, np& t);
```

## 計算量

代表探索・併合 O(log² N) を上界とする。永続配列更新は O(log N) ノードを追加。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/persistent_unionfind_with_data`](../../../verify/library_checker/data_structure/persistent_unionfind_with_data.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
