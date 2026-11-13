# nimber

[索引](../README.md) / [実装](../../../library/math/nimber.hpp)

## 機能・前提条件

64bit nim積に対応する体要素。通常整数表現との変換はget()を使用する。

## インターフェース

```cpp
#include "library/math/nimber.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct Nimber;
using u64 = uint64_t;
using u128 = __uint128_t;
Nimber();
Nimber(u64 x_);
inline u64 get() const;
friend Nimber operator+(Nimber a, Nimber b);
friend Nimber operator-(Nimber a, Nimber b);
friend Nimber operator*(Nimber a, Nimber b);
friend Nimber operator/(Nimber a, Nimber b);
friend Nimber& operator+=(Nimber& a, Nimber b);
friend Nimber& operator-=(Nimber& a, Nimber b);
friend Nimber& operator*=(Nimber& a, Nimber b);
friend Nimber& operator/=(Nimber& a, Nimber b);
Nimber operator-() const;
Nimber inv() const;
Nimber pow(u64 b) const;
friend bool operator==(Nimber a, Nimber b);
friend bool operator!=(Nimber a, Nimber b);
friend bool operator<(Nimber a, Nimber b);
friend bool operator>(Nimber a, Nimber b);
friend bool operator<=(Nimber a, Nimber b);
friend bool operator>=(Nimber a, Nimber b);
friend ostream& operator<<(ostream& os, const Nimber& a);
friend istream& operator>>(istream& is, Nimber& a);
```

## 計算量

64bit固定幅で各演算 O(1)、pow O(log k) 回の乗算、値の空間 O(1)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/nim_product_64`](../../../verify/library_checker/number_theory/nim_product_64.cpp)
- [`unit/math_misc`](../../../verify/unit/math_misc.cpp)
