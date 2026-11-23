# internal_math

[索引](../../README.md) / [実装](../../../../library/math/internal/internal_math.hpp)

## 機能・前提条件

符号付き除算の丸め、剰余、Barrett/Montgomery、mod冪、素数判定などの内部数論。

## インターフェース

```cpp
#include "library/math/internal/internal_math.hpp"
```

- `soryuusi::divfloor(a,b)` と `soryuusi::divceil(a,b)` は符号付き整数でも商を数学的な床・天井に丸める。`b != 0` を前提とする。
- `internal::safe_mod(x,m)` は `m >= 1` のもとで剰余を `[0,m)` に正規化する。
- `internal::barrett` と `internal::montgomery64` は固定法の積・剰余を扱う。Montgomery版の `transform`、`value` は通常表現と演算用表現の間を変換する。
- `internal::pow_mod_constexpr`、`internal::pow_mod64_constexpr`、`internal::mod_pow` は整数のmod冪を扱う。
- `internal::is_prime_constexpr`、`internal::is_prime64_constexpr`、`internal::is_prime64_runtime` は整数の素数判定を行う。
- `internal::inv_gcd(a,b)` はgcdと対応する剰余類での逆元を返す。`internal::primitive_root_constexpr(m)` は素数法の原始根を返す。
- `internal::ceil_pow2(n)` と `internal::bsf(n)` は共通のビット操作で2冪の指数と下位の立っているビットの位置を求める。

## 計算量

基本演算 O(1)、冪 O(log k)、gcd系 O(log m)。固定幅と中間値の制約を満たすこと。

## 依存関係

- [internal_bit.hpp](internal_bit.md)

## Verify・利用例

- [`library_checker/primality_test_internal_math`](../../../../verify/library_checker/number_theory/primality_test_internal_math.cpp)
