# assignment_problem

[索引](../README.md) / [実装](../../../library/graph/assignment_problem.hpp)

## 機能・前提条件

費用行列の各行を異なる列に割り当て、総費用を最小化する。行数 n、列数 m は独立で、n > m のときは実行不能。費用は 64 bit 以下の符号付き整数型。負費用・同値費用・禁止辺を扱う。禁止辺は選択可否のマスクで指定する。

## インターフェース

```cpp
#include "library/graph/assignment_problem.hpp"

std::vector<std::vector<int>> cost{{1,-3,4},{0,1,8}};
auto result = soryuusi::assignment_problem(cost);
// result->cost == -3, result->left_to_right == {1,0}
```

- `assignment_problem(costs[,allowed])` は `vector<vector<Cost>>` と任意の同形 `vector<vector<bool>>` を取る。 `assignment_problem<Cost>(n,m,costs[,allowed])` は行優先の `span<const Cost>` と任意の `span<const unsigned char>` を取る。マスクの真または非零が許可。形が不正なら例外、Hall 条件違反などで割当てが存在しなければ `nullopt`。
- `AssignmentResult::cost` は総費用。 `left_to_right` と `right_to_left` は割当ての相手番号で、未割当ての列は −1。最適解が複数ある場合はそのうち一つを返す。
- `left_potential[i]+right_potential[j] <= costs[i][j]` が許可辺上で成立し、割当て辺は等号。 `right_potential[j] <= 0` で未使用列は 0。費用と双対変数の出力は `__int128_t`。

## 計算量

Hungarian 法で最悪 O(n²m)、入力以外の作業領域 O(n+m)。双対実行可能な行最小値から開始する。マスク省略の正方行列では Monge 性の検査・列リダクションも使う。Monge または逆 Monge と判定した場合は O(n²) で割当てと双対を得る。疎な問題は許可辺だけを [最小費用流](min_cost_flow.md) に渡す帰着も使える。

## 依存関係

標準ライブラリ。

## Verify・利用例

- [`library_checker/assignment`](../../../verify/library_checker/graph/assignment.cpp)
- [`unit/assignment_problem`](../../../verify/unit/assignment_problem.cpp) は長方形、禁止辺、64 bit 境界で費用と双対を検査する。
