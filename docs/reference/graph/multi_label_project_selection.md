# multi_label_project_selection

[索引](../README.md) / [実装](../../../library/graph/multi_label_project_selection.hpp)

## 機能・前提条件

変数ごとに順序付きラベル `[0,k_i)` を選び、単項費用と Monge な二変数費用の合計を最小化する。ラベル a は `label[i] >= a` という閾値変数で表す。各変数のラベル数は 1 以上。

二変数費用行列の隣接 2×2 ブロックは `f[a-1][b-1] + f[a][b] <= f[a-1][b] + f[a][b-1]` を満たす必要がある。各行・列は対応する変数のラベル順に並べる。`Cost` は 64 bit 以下の符号付き整数型。累積費用・差分・ハード制約用の容量は符号付き 128 bit に収める。

## インターフェース

```cpp
#include "library/graph/multi_label_project_selection.hpp"

soryuusi::MultiLabelProjectSelection<> g({3,2});
g.add_pair_cost(0,1,{{0,1},{1,0},{2,1}});
g.fix(0,2);
auto answer = g.solve();   // cost == 1, label == {2,1}
```

- `add_constant(c)` は定数費用。`add_unary_cost(i,costs)` はラベル順の費用を `span<const Cost>` で受け取る。
- `add_pair_cost(i,j,table)` は `k_i × k_j` の `vector<vector<Cost>>` を参照で受け取る。`i != j` を要求する。サイズの不一致または Monge 条件違反は `std::invalid_argument`。Monge 条件を確認してからモデルに加える。
- `fix(i,a)` はラベル a への固定。ラベル 1 個の変数にも使える。
- `solve() const` は Push-Relabel、`solve_dinic() const` は Dinic を使う。矛盾時は `nullopt`、実行可能なら `Result{cost,label}` を返す。ラベルごとの成分が最小となる同値解を返す。戻り値の型は費用が `__int128_t`、ラベル列が `vector<int>`。
- `reserve_edges(m)` は閾値の二変数の有限容量辺に使う保持領域を予約する。頂点数と逆辺を含む辺数は `int` の範囲を使う。

## 計算量

頂点数は `2+sum(k_i-1)`。各変数に対し `max(k_i-2,0)` 本の順序制約辺を張る。単項費用の登録は O(k_i)。二変数費用の検査・登録は O(k_i k_j) で、隣接差分が非零の組にだけ有限容量辺を張る。上界は `(k_i-1)(k_j-1)` 本。solve 時のグラフ構築・復元は O(V+E)、最大流は最悪 O(V²E)、空間は O(V+E)。ここで E は順序・固定・単項の辺も含む。

## 依存関係

- [internal/selection_cut.hpp](internal/selection_cut.md)

## Verify・利用例

- [`unit/project_selection`](../../../verify/unit/project_selection.cpp) は符号付き Monge 行列、偏ったラベル数、ARC129 E の例題を検査する。
- 数値ラベル間の絶対差をペナルティとする応用例。 [ARC129 E](https://atcoder.jp/contests/arc129/tasks/arc129_e)
