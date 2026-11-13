# dsu_undo

[索引](../README.md) / [実装](../../../library/data_structure/dsu_undo.hpp)

## 機能・前提条件

巻き戻し可能なUnion-Find。get_state と rollback、直近操作の undo を使う。経路圧縮しない。

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

merge/leader/same/size O(log N)、undo O(1)、rollback は戻す履歴数に比例。空間 O(N+Q)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

直接includeするverifyはありません。依存先からの間接利用は `make report` で確認できます。
