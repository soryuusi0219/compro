# dsu_undo_with_data

[索引](../README.md) / [実装](../../../library/data_structure/dsu_undo_with_data.hpp)

## 機能・前提条件

成分データ付き巻き戻しUnion-Find。op で値を結合し、apply で成分へ値を加える。snapshot/rollback に対応。

## インターフェース

```cpp
#include "library/data_structure/dsu_undo_with_data.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, auto op>
struct dsu_undo_data;
dsu_undo_data();
dsu_undo_data(int n_, T v);
dsu_undo_data(const vector<T>& val_);
int merge(int a, int b);
void apply(int a, T v);
bool same(int a, int b);
int leader(int a);
int size(int a);
T get(int a);
vector<vector<long long>> groups();
int get_state();
void snapshot();
void undo();
void rollback(int state = -1);
```

## 計算量

代表探索・併合 O(log N)、undo は1操作分、空間 O(N+Q)。op の実行時間を別途掛ける。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

直接includeするverifyはありません。依存先からの間接利用は `make report` で確認できます。
