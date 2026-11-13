# fps_light

[索引](../../README.md) / [実装](../../../../library/math/light/fps_light.hpp)

## 機能・前提条件

軽量なFPS実装。inv/diff/integral/log/exp/powを提供する。通常FormalPowerSeriesとAPI・機能範囲が異なる。

## インターフェース

```cpp
#include "library/math/light/fps_light.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
using ll = long long;
using vll = vector<ll>;
template <class Mint, internal::is_static_modint_like_t<Mint>* = nullptr>
struct FPS : vector<Mint>;
using mint = Mint;
using vm = vector<mint>;
using vm::vm;
using vm::begin;
using vm::end;
using vm::front;
using vm::insert;
using vm::resize;
using vm::size;
FPS(const vm& v);
FPS(const vll& v);
FPS operator+=(const FPS& x);
FPS operator-=(const FPS& x);
FPS operator*=(const mint& m);
FPS operator/=(const mint& m);
FPS operator+(const FPS& x) const;
FPS operator-(const FPS& x) const;
FPS operator-() const;
FPS operator*(mint m) const;
FPS operator/(mint m) const;
FPS operator*(const FPS& x) const;
FPS operator/(const FPS& x) const;
FPS operator*=(const FPS& x);
FPS operator/=(const FPS& other);
FPS pre(ll k) const;
FPS inv(ll N = -1) const;
FPS diff() const;
FPS integral() const;
FPS log(ll N = -1) const;
FPS exp(ll N = -1) const;
FPS pow(ll k, ll N = -1) const;
```

## 計算量

積をM(N)としてNewton操作 O(M(N) log N) を上界とする。微積分 O(N)、空間 O(N) を基本とする。

## 依存関係

- [convolution.hpp](../convolution.md)
- [modint_traits.hpp](../modint_traits.md)

## Verify・利用例

- [`library_checker/exp_of_formal_power_series_light`](../../../../verify/library_checker/polynomial/exp_of_formal_power_series_light.cpp)
- [`library_checker/inv_of_formal_power_series_light`](../../../../verify/library_checker/polynomial/inv_of_formal_power_series_light.cpp)
- [`library_checker/log_of_formal_power_series_light`](../../../../verify/library_checker/polynomial/log_of_formal_power_series_light.cpp)
- [`library_checker/pow_of_formal_power_series_light`](../../../../verify/library_checker/polynomial/pow_of_formal_power_series_light.cpp)
- [`unit/math_light`](../../../../verify/unit/math_light.cpp)
