# range_parallel_unionfind

[索引](../README.md) / [実装](../../../library/data_structure/range_parallel_unionfind.hpp)

## 機能・前提条件

同じ長さの二つの半開区間 `[l,l+x)` と `[r,r+x)` を、先頭から対応する位置ごとに併合するUnion-Find。`x = 0` では何も起こらない。コールバックは実際に異なる成分が併合されたときに、併合前の2つの代表元と併合後の代表元を受け取る。`N = 0` の `groups()` は空列を返す。

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

構築・空間は `O(N log(N+1))`。区間長の立っているビットごとに対応する階層を探索し、その階層で新たに併合した場合だけ子へ進む。`Q` 回の区間併合の合計は `O((N+Q) log(N+1) α(N))`、長さ `x` の併合1回の最悪は `O(x log(N+1))`。コールバックの実行時間は別に数える。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)
- [dsu.hpp](dsu.md)

## Verify・利用例

- [`unit/unionfind_hash`](../../../verify/unit/unionfind_hash.cpp)
- [`library_checker/range_parallel_unionfind`](../../../verify/library_checker/data_structure/range_parallel_unionfind.cpp)
