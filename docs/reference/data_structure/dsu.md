# dsu

[索引](../README.md) / [実装](../../../library/data_structure/dsu.hpp)

## 機能・前提条件

連結成分の併合・代表元・サイズ。merge は新しい代表元、groups は成分の頂点列を返す。

## インターフェース

```cpp
#include "library/data_structure/dsu.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct DSU;
DSU();
DSU(int n);
int leader(int u);
bool same(int u, int v);
int size(int u);
int merge(int u, int v);
vector<vector<int>> groups();
```

## 計算量

構築 O(N)、通常操作は償却 O(α(N))、groups O(N α(N))、空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/unionfind`](../../../verify/library_checker/data_structure/unionfind.cpp)
