# gf2

[索引](../README.md) / [実装](../../../library/math/gf2.hpp)

## 機能・前提条件

GF(2^D)のスカラー演算。GF2<D>、0<D<64。既約多項式表を用いる。値はD bitに収め、除算の分母は非零とする。

## インターフェース

```cpp
#include "library/math/gf2.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<int D>
struct GF2;
using u64 = uint64_t;
static constexpr u64 mul(u64 x, u64 y);
GF2(u64 x_ = 0);
friend GF2 operator+(GF2 a, GF2 b);
friend GF2 operator-(GF2 a, GF2 b);
friend GF2 operator*(GF2 a, GF2 b);
friend GF2 operator/(GF2 a, GF2 b);
friend GF2& operator+=(GF2& a, GF2 b);
friend GF2& operator-=(GF2& a, GF2 b);
friend GF2& operator*=(GF2& a, GF2 b);
friend GF2& operator/=(GF2& a, GF2 b);
friend bool operator==(const GF2& a, const GF2& b);
friend bool operator!=(const GF2& a, const GF2& b);
friend bool operator<(const GF2& a, const GF2& b);
friend bool operator>(const GF2& a, const GF2& b);
friend bool operator<=(const GF2& a, const GF2& b);
friend bool operator>=(const GF2& a, const GF2& b);
GF2 operator-() const;
friend ostream& operator<<(ostream& os, const GF2& a);
friend istream& operator>>(istream& is, GF2& a);
friend GF2& operator<<=(GF2& a, u64 k);
friend GF2 operator<<(GF2 a, u64 k);
GF2 inv() const;
GF2 pow(u64 b) const;
GF2 sqrt() const;
```

## 計算量

加減算 O(1)、乗算 O(D)、逆元・冪 O(D log k)、空間 O(1)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/math_misc`](../../../verify/unit/math_misc.cpp)
