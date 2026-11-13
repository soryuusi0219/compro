# rand

[索引](../README.md) / [実装](../../../library/utility/rand.hpp)

## 機能・前提条件

`random_integer<Int>(lower,upper)` で閉区間 `[lower,upper]` の一様整数乱数を得る。`lower<=upper` が必要。共有 `random_engine` はmt19937。

## インターフェース

```cpp
#include "library/utility/rand.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
inline std::mt19937 random_engine(std::random_device{}());
template <class Int>
Int random_integer(Int lower, Int upper);
```

## 計算量

固定幅の整数に期待 O(1) 時間、O(1) 状態。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/utility_misc`](../../../verify/unit/utility_misc.cpp)
