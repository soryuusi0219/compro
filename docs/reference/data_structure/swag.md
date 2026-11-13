# swag

[索引](../README.md) / [実装](../../../library/data_structure/swag.hpp)

## 機能・前提条件

結合的演算の列キュー集約。pushで末尾追加、popで先頭削除、prodで列全体の積。

## インターフェース

```cpp
#include "library/data_structure/swag.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<typename T, auto op, auto e>
struct swag;
swag();
inline bool empty() const;
inline int size() const;
void p_to_s();
void push(T x);
void pop();
T prod() const;
```

## 計算量

push/pop 償却 O(1)、prod O(1)、空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/queue_operate_all_composite`](../../../verify/library_checker/data_structure/queue_operate_all_composite.cpp)
