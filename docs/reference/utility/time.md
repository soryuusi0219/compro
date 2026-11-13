# time

[索引](../README.md) / [実装](../../../library/utility/time.hpp)

## 機能・前提条件

tick()による区間実行時間の簡易計測。出力先と初回との差分は実装を確認して利用する。

## インターフェース

```cpp
#include "library/utility/time.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
void tick();
```

## 計算量

1回 O(1) 時間・空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/utility_misc`](../../../verify/unit/utility_misc.cpp)
