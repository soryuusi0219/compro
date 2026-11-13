# set_power_series

[索引](../README.md) / [実装](../../../library/math/set_power_series.hpp)

## 機能・前提条件

部分集合畳み込み上の冪級数。inv/exp/log/pow、power projection、composition。入力長は2冪。

## インターフェース

```cpp
#include "library/math/set_power_series.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
vector<T> convolution_subset(const vector<T>& a, const vector<T>& b);
template<typename T>
struct SetPowerSeries : vector<T>;
using vector<T>::vector;
using SPS = SetPowerSeries;
template<class U> SetPowerSeries(const vector<U>& v);
inline int dim() const;
friend SPS& operator+=(SPS& x, const SPS& y);
friend SPS& operator-=(SPS& x, const SPS& y);
friend SPS& operator*=(SPS& x, const T y);
friend SPS& operator/=(SPS& x, const T y);
friend SPS operator+(SPS x, const SPS& y);
friend SPS operator-(SPS x, const SPS& y);
friend SPS operator*(SPS x, const T y);
friend SPS operator/(SPS x, const T y);
SPS operator-() const;
friend SPS operator*(const SPS& x, const SPS& y);
friend SPS& operator*=(SPS& x, const SPS& y);
friend SPS operator/(const SPS& x, const SPS& y);
friend SPS& operator/=(SPS& x, const SPS& y);
SPS inv() const;
SPS exp() const;
SPS log() const;
SPS pow(int m) const;
SPS pow_rational(T m) const;
SPS sqrt() const;
template<class T, class Factorials = modint_factorials<T>>
vector<T> power_projection(const SetPowerSeries<T>& f, const SetPowerSeries<T>& g, int m);
template<class T>
SetPowerSeries<T> composition(const vector<T>& F, const SetPowerSeries<T>& f);
```

## 計算量

L=2^B、積 O(B²L)、他の操作は部分集合畳み込みの反復に依存し O(B³L) を基本的な上界とする。空間 O(BL)。

## 依存関係

- [modint_traits.hpp](modint_traits.md)
- [modint_factorials.hpp](modint_factorials.md)

## Verify・利用例

- [`library_checker/exp_of_set_power_series`](../../../verify/library_checker/set_power_series/exp_of_set_power_series.cpp)
- [`library_checker/polynomial_composite_set_power_series`](../../../verify/library_checker/set_power_series/polynomial_composite_set_power_series.cpp)
- [`library_checker/power_projection_of_set_power_series`](../../../verify/library_checker/set_power_series/power_projection_of_set_power_series.cpp)
