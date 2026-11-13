# enumerate_cliques

[索引](../README.md) / [実装](../../../library/graph/enumerate_cliques.hpp)

## 機能・前提条件

無向単純グラフのクリークをcallbackへ列挙する。MAXNを静的bitset幅に指定する。weighted_sumで重み積の総和を求める。

## インターフェース

```cpp
#include "library/graph/enumerate_cliques.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<int MAXN>
struct EnumerateCliques;
using bit = bitset<MAXN>;
EnumerateCliques(int n_);
void add_edge(int u, int v);
template<class F>
void solve(F f);
template<class T>
T weighted_sum(const vector<T>& weight);
```

## 計算量

出力数 K に依存する指数時間（最悪 O(poly(V) 2^V)）、空間は隣接bit列と再帰状態。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/enumerate_cliques`](../../../verify/library_checker/graph/enumerate_cliques.cpp)
