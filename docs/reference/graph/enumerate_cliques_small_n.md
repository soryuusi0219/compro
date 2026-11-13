# enumerate_cliques_small_n

[索引](../README.md) / [実装](../../../library/graph/enumerate_cliques_small_n.hpp)

## 機能・前提条件

128bit整数で隣接を保持する小頂点数版クリーク列挙。通常版と EnumerateCliques の名前が衝突する。

## インターフェース

```cpp
#include "library/graph/enumerate_cliques_small_n.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct EnumerateCliques;
using bit = __uint128_t;
static inline int ctz(bit x);
EnumerateCliques(int n_);
void add_edge(int u, int v);
template<class F>
void solve(F f);
template<class T>
T weighted_sum(const vector<T>& weight);
```

## 計算量

最悪 O(poly(V) 2^V) 時間、隣接 O(V) word と再帰状態。V≤128のbit幅内で使う。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/enumerate_cliques_small_n`](../../../verify/library_checker/graph/enumerate_cliques_small_n.cpp)
