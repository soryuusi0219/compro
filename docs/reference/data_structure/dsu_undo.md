# dsu_undo

[索引](../README.md) / [実装](../../../library/data_structure/dsu_undo.hpp)

## 機能・前提条件

巻き戻し可能なUnion-Find。状態番号は現在保持している成功した併合数。`merge` は既に同じ成分なら `-1` を返す。`undo` は直近に成功した併合を1件戻す。`rollback(state)` には現在の状態番号以下の有効な状態番号を渡す。親探索では経路圧縮せず、サイズによる併合で深さを抑える。

## インターフェース

```cpp
#include "library/data_structure/dsu_undo.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct dsu_undo;
dsu_undo();
dsu_undo(int n_);
int merge(int a, int b);
bool same(int a, int b);
int leader(int a);
int size(int a);
vector<vector<long long>> groups();
int get_state();
void undo();
void rollback(int state);
```

## 計算量

`merge`・`leader`・`same`・`size` は最悪 `O(log(N+1))`、`undo` は `O(1)`、`rollback` は戻す履歴数に比例する。未取消の成功した併合は最大 `N-1` 件で、空間は `O(N)`。成分列挙は最悪 `O(N log(N+1))`。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/unionfind_hash`](../../../verify/unit/unionfind_hash.cpp)
