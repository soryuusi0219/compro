# dynamic_dsu

[索引](../README.md) / [実装](../../../library/data_structure/dynamic_dsu.hpp)

## 機能・前提条件

疎な頂点IDをハッシュ表で管理するUnion-Find。コンストラクタの容量を超えない範囲で利用する。

## インターフェース

```cpp
#include "library/data_structure/dynamic_dsu.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct dynamic_dsu;
dynamic_dsu(int n);
int leader(int u);
bool same(int u, int v);
int size(int u);
int merge(int u, int v);
```

## 計算量

ハッシュが期待 O(1) のとき通常操作は償却 O(α(N))、空間は確保容量に比例。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/unionfind_dynamic`](../../../verify/library_checker/data_structure/unionfind_dynamic.cpp)
