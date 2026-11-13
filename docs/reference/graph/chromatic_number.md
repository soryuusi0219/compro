# chromatic_number

[索引](../README.md) / [実装](../../../library/graph/chromatic_number.hpp)

## 機能・前提条件

部分集合DP・包除原理による彩色数。bit集合に収まる小さい頂点数に使う。

## インターフェース

```cpp
#include "library/graph/chromatic_number.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct chromaticnumber;
chromaticnumber(int node_size);
void add_edge(int u, int v);
void remove_edge(int u, int v);
int color();
```

## 計算量

O(V 2^V) 時間、O(2^V) 空間。内部固定整数modによる判定を使う。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/chromatic_number`](../../../verify/library_checker/graph/chromatic_number.cpp)
