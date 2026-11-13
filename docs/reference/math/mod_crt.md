# mod_crt

[索引](../README.md) / [実装](../../../library/math/mod_crt.hpp)

## 機能・前提条件

CRT APIへの互換include。実装はmath.hppに集約されている。

## インターフェース

```cpp
#include "library/math/mod_crt.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
// 互換include。依存先のAPIを参照。
```

## 計算量

math.hppのCRTと同じ。入力法の数Kに対しgcd処理をK回。

## 依存関係

- [math.hpp](math.md)

## Verify・利用例

- [`library_checker/sum_of_floor_of_linear`](../../../verify/library_checker/number_theory/sum_of_floor_of_linear.cpp)
