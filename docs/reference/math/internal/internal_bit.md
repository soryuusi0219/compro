# internal_bit

[索引](../../README.md) / [実装](../../../../library/math/internal/internal_bit.hpp)

## 機能・前提条件

整数のbit幅・2冪切上げ・末尾零・popcountなどの移植用補助。主に内部向けで型制約を持つ。

## インターフェース

```cpp
#include "library/math/internal/internal_bit.hpp"
```

ビット数を扱う関数の引数には64ビットまでの符号なし整数を渡す。

- `bit_width(x)`、`bit_floor(x)`、`bit_ceil(x)`、`has_single_bit(x)` はビット幅と2冪を扱う。
- `countl_zero(x)`、`countr_zero(x)`、`countl_one(x)`、`countr_one(x)` は端から連続するビットを数える。ゼロの先頭・末尾のゼロ数は引数型のビット幅。
- `popcount(x)` のほか、`rotl`、`rotr`、`byteswap`、`bit_cast` を提供する。
- `bit_reverse`、`bit_repeat`、`bit_compress`、`bit_expand`、`shl`、`shr` はビット列の変換を行う。

関数は `soryuusi::` で修飾して呼ぶ。標準ライブラリ、処理系の組み込み関数、代替実装はヘッダーが選択する。

## 計算量

固定幅整数で O(1)。fallbackは整数bit幅に比例する場合がある。空間 O(1)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/internal_bit`](../../../../verify/unit/internal_bit.cpp)
