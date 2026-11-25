# swag_deque

[索引](../README.md) / [実装](../../../library/data_structure/swag_deque.hpp)

## 機能・前提条件

両端の追加削除に対応するSWAG。非可換演算でも先頭から末尾の順に集約します。片側を使い切った場合はもう一方を分け直し、元の側に残す前半だけを一時配列に置きます。

## インターフェース

```cpp
#include "library/data_structure/swag_deque.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<typename T, auto op, auto e>
struct swagdq;
swagdq();
bool empty();
int size();
void p_to_s();
void s_to_p();
void push_front(T x);
void push_back(T x);
void pop_front();
void pop_back();
T prod();
```

## 計算量

両端操作は償却 O(1)、分け直しが発生する単発の最悪は O(N)。prodは O(1)、空間は O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/deque_operate_all_composite`](../../../verify/library_checker/data_structure/deque_operate_all_composite.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
