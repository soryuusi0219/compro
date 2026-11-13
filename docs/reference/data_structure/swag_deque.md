# swag_deque

[索引](../README.md) / [実装](../../../library/data_structure/swag_deque.hpp)

## 機能・前提条件

両端の追加削除に対応するSWAG。非可換演算でも列順を保って全体を集約する。

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

両端操作は償却 O(1)、prod O(1)、空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/deque_operate_all_composite`](../../../verify/library_checker/data_structure/deque_operate_all_composite.cpp)
