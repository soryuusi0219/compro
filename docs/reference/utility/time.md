# time

[索引](../README.md) / [実装](../../../library/utility/time.hpp)

## 機能・前提条件

`tick()` は `std::clock()` でプロセスのCPU時間を測り、ミリ秒を `std::cout` に出力してflushする。初回は `clock()` の基準時点から、以後は前回の出力を終えた時点からの差。出力は小数点以下15桁の固定小数点で、`std::cout` の書式設定にも反映される。

## インターフェース

```cpp
#include "library/utility/time.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
inline void tick();
```

## 計算量

1回 O(1) 時間・空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/utility_misc`](../../../verify/unit/utility_misc.cpp)
