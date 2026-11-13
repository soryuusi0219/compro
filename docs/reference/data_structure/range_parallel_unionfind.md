# range_parallel_unionfind

[索引](../README.md) / [実装](../../../library/data_structure/range_parallel_unionfind.hpp)

## 機能・前提条件

同じ長さの二つの区間を位置ごとに併合するUnion-Find。実際に併合された要素にcallbackを呼べる。

## インターフェース

```cpp
#include "library/data_structure/range_parallel_unionfind.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct Range_Parallel_Unionfind;
Range_Parallel_Unionfind(int n_);
int leader(int u);
bool same(int u, int v);
vector<vector<int>> groups();
void _merge(int l, int r, int i);
void merge(int l, int r, int x);
template<class F>
void _merge(int l, int r, int i, F f);
template<class F>
void merge(int l, int r, int x, F f);
```

## 計算量

初期化 O(N log N)、全区間併合で O(N log N α(N)+Q α(N)) を上界とする。空間 O(N log N)。

## 依存関係

- [internal_bit.hpp](../math/internal_bit.md)
- [dsu.hpp](dsu.md)

## Verify・利用例

- [`library_checker/range_parallel_unionfind`](../../../verify/library_checker/data_structure/range_parallel_unionfind.cpp)
