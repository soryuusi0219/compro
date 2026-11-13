# heavy_light_decomposition_balanced

[索引](../README.md) / [実装](../../../library/tree/heavy_light_decomposition_balanced.hpp)

## 機能・前提条件

静的木のパス集約を平衡化したHeavy-Light構造で保持する。一点更新と非可換なパス積に対応する。

## インターフェース

```cpp
#include "library/tree/heavy_light_decomposition_balanced.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e>
struct HLdecomposition_Balanced;
HLdecomposition_Balanced(int n_);
void add_edge(int u, int v);
void build(int root = 0, const vector<S>& data = {});
void set(int u, S x);
S get(int u);
S prod(int u, int v);
```

## 計算量

構築 O(N log N)、更新・パス積 O(log² N) を保守的上界とする。空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/vertex_add_path_sum`](../../../verify/library_checker/tree/vertex_add_path_sum.cpp)
