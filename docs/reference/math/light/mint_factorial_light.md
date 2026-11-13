# mint_factorial_light

[索引](../../README.md) / [実装](../../../../library/math/light/mint_factorial_light.hpp)

## 機能・前提条件

型ごとの共有階乗表とnPk/nCkの軽量関数。mf_initで十分な範囲を前計算する。

## インターフェース

```cpp
#include "library/math/light/mint_factorial_light.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
using ll = long long;
template <class Mint>
template <class Mint>
template <class Mint>
template <class Mint>
void mf_init(ll n);
template <class Mint>
Mint nPk(ll n, ll k);
template <class Mint>
Mint nCk(ll n, ll k);
```

## 計算量

前計算 O(N)、問い合わせ O(1)、型ごと O(N) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/math_light`](../../../../verify/unit/math_light.cpp)
