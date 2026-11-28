# project_selection

[索引](../README.md) / [実装](../../../library/graph/project_selection.hpp)

## 機能・前提条件

0/1 変数の費用とハード制約を最小 s-t カットに帰着する。最小化で統一し、利益を登録するとその額を費用から引く。source 側の変数が真となる。

`Cost` は 64 bit 以下の符号付き整数型。費用の差、累積費用、ハード制約用の容量は符号付き 128 bit に収める。変数は `[0,n)`。補助頂点を含む頂点数と逆辺を含む辺数を `int` で管理する。

## インターフェース

```cpp
#include "library/graph/project_selection.hpp"

soryuusi::ProjectSelection<> g(3);
g.add_profit(0,10);
g.add_cost(1,4);
g.imply(0,1);
g.add_all_true_profit({0,2},3);
auto answer = g.solve();   // cost == -9, selected == {true,true,true}
```

- `add_constant(c)` は定数費用。`add_unary_cost(i,c0,c1)` は値ごとの費用。`add_cost(i,c,value=true)` と `add_profit(i,p,value=true)` は指定値のときに費用を足す、または利益を引く。単項の費用と利益は負値も受け付ける。
- `add_pair_cost(i,j,f00,f01,f10,f11)` は二変数の条件付き費用。劣モジュラ条件 `f00+f11 <= f01+f10` を満たす必要があり、違反時は `std::invalid_argument`。同じ変数を指定した場合は条件を検査してから対角成分を単項費用として使う。
- `add_implication_penalty(i,j,p)` は `i=true,j=false` のときの費用。`add_disagreement_cost(i,j,p)` は値が異なるときの費用。いずれも `p >= 0`。
- `imply(i,j)` は真の含意、`equal(i,j)` は等値制約、`fix(i,value)` は値の固定。
- `add_all_true_profit(indices,p)` と `add_all_false_profit(indices,p)` は全変数が指定値のときに非負の利益を得る。`indices` は `span<const int>` または初期化リスト。空集合では常に利益が入る。重複添字は同じ変数への追加条件として扱う。
- `solve() const` は Dinic、`solve_push_relabel() const` は Push-Relabel を使う。どちらも矛盾時は `nullopt`、実行可能なら `Result{cost,selected}` を返す。同値解では真の変数集合が包含関係で最小となる解を返す。モデルを保持し、条件を追加して再度解ける。
- `reserve_edges(m)` はモデルに保持する二変数の有限容量辺の領域を予約する。単項の辺は変数ごとに集約して solve 時に加える。

## 計算量

単項費用・二変数費用・ハード制約の追加は償却 O(1)。グループ利益は変数 k 個に対し O(k)、2 個以上なら補助変数を 1 つ追加する。

補助頂点・端点を含む頂点数を V、生成した辺数を E とする。グラフ構築・復元は O(V+E)、最大流は最悪 O(V²E)、空間は O(V+E)。有限容量の合計に 1 足した値をハード辺の容量とするため、最小カットがこの値以上なら矛盾。値の固定が両側にある場合は、まずハード辺だけで矛盾を判定する。容量が `uint64_t` に収まる場合は 64 bit の残余グラフを使う。

## 依存関係

- [internal/selection_cut.hpp](internal/selection_cut.md)

## Verify・利用例

- [`aoj/project_selection`](../../../verify/aoj/graph/project_selection.cpp) で AOJ 3058 の最小費用を求める。
- [`unit/project_selection`](../../../verify/unit/project_selection.cpp) は全割当てとの一致、同値解、矛盾、128 bit の費用を検査する。
