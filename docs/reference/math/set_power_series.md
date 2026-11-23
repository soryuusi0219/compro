# set_power_series

[索引](../README.md) / [実装](../../../library/math/set_power_series.hpp)

## 機能・前提条件

部分集合畳み込みを積とする冪級数。係数は集合を表すビットマスク順に格納し、長さは2の冪とする。`vector` から構築すると長さを次の2冪まで0で埋める。底の環の係数演算は可換とする。

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
SetPowerSeries(vector<T>&& v);
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

- `inv`、級数による除算、負の整数指数の `pow` では、逆数を取る級数の空集合係数が可逆であること。非負の整数指数は空集合係数が0でも使える。
- `exp` は空集合係数0、`log` と `pow_rational` は空集合係数1を要求する。`sqrt` は `pow_rational(1/2)` で、2が可逆であることも必要。
- `pow_rational(m)` では集合を $r$ 個の非空ブロックに分割した項に、係数 $m(m-1)\cdots(m-r+1)$ を掛ける。
- `composition(F,f)` は普通の多項式 $F$ を部分集合畳み込みの代数で $f$ に代入する。$f[0]$ が非零で非可逆でも使える。
- `power_projection(f,g,m)` は $0 \le i < m$ に対して、$g f^i$ の全体集合の係数を返す。`f` と `g` は同じ長さを渡す。

## 計算量

長さを $L=2^B$ とする。積、除算、逆数、指数、対数、平方根、冪は $O((B+1)^2 L)$ 時間。整数冪には空集合係数の冪を計算する $O(\log(1+|m|))$ が加わる。

合成とpower projectionは、同じ長さで繰り返す部分集合畳み込みの変換表を再利用する。合成の時間は、外側の係数数を $M$ として $O((B+1)^2 L+(B+1)M)$。Power projectionは $M$ を要求する係数数として同じ上界を持つ。`f[0]` が0の場合、合成では外側の先頭 $B+1$ 係数だけを読み、power projectionでは $B+1$ 番目以降を0で埋める。作業領域は $O((B+1)L)$ で、`f[0]` が非零のpower projectionには係数の冪を保持する $O(M)$ が加わる。

## 依存関係

- [convolution_subset.hpp](convolution_subset.md)
- [modint_factorials.hpp](modint_factorials.md)
- [internal_bit.hpp](internal/internal_bit.md)

## Verify・利用例

- [`unit/set_power_series`](../../../verify/unit/set_power_series.cpp)
- [`library_checker/subset_convolution`](../../../verify/library_checker/set_power_series/subset_convolution.cpp)
- [`library_checker/exp_of_set_power_series`](../../../verify/library_checker/set_power_series/exp_of_set_power_series.cpp)
- [`library_checker/log_of_set_power_series`](../../../verify/library_checker/set_power_series/log_of_set_power_series.cpp)
- [`library_checker/polynomial_composite_set_power_series`](../../../verify/library_checker/set_power_series/polynomial_composite_set_power_series.cpp)
- [`library_checker/power_projection_of_set_power_series`](../../../verify/library_checker/set_power_series/power_projection_of_set_power_series.cpp)
- [`library_checker/chromatic_polynomial`](../../../verify/library_checker/graph/chromatic_polynomial.cpp)
