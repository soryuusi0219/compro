# dsu_undo_with_data

[索引](../README.md) / [実装](../../../library/data_structure/dsu_undo_with_data.hpp)

## 機能・前提条件

成分データ付き巻き戻しUnion-Find。`merge` は残る代表元の値を左、吸収される成分の値を右にして `op` で結合する。`apply` は成分の値を左にして指定した値を `op` で結合する。成功した併合または適用ごとに状態番号を進め、`undo` は1操作分戻す。引数を省略した `rollback()` は最後に `snapshot()` した有効な状態へ戻る。

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

代表探索・併合・適用の親探索は最悪 `O(log(N+1))`。一操作の `undo` は最大2件の履歴を復元し、`rollback` は取り消す操作数に比例する。値の演算・コピー・代入の時間は別に数える。未取消の操作を `Q` 件として空間 `O(N+Q)`。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/unionfind_hash`](../../../verify/unit/unionfind_hash.cpp)
