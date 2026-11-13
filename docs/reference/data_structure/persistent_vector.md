# persistent_vector

[索引](../README.md) / [実装](../../../library/data_structure/persistent_vector.hpp)

## 機能・前提条件

添字のbitを辿る永続配列。set の返したrootを保持し、des_set は破壊的設定として使い分ける。

## インターフェース

```cpp
#include "library/data_structure/persistent_vector.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<typename T>
struct Persistent_Vector;
struct node;
using np=node*;
struct node;
Persistent_Vector();
np get_root();
void des_set(int id, T val, np& t);
np set(int id, T val, np& t);
np mutable_get(int id, np t);
T get(int id, np t);
```

## 計算量

添字 i に対し取得・更新 O(log(i+1))、更新ごと同程度のノード追加。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

直接includeするverifyはありません。依存先からの間接利用は `make report` で確認できます。
