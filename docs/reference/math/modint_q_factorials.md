# modint_q_factorials

[索引](../README.md) / [実装](../../../library/math/modint_q_factorials.hpp)

## 機能・前提条件

固定した素数法 $p$ と $q\in\mathbb F_p$ について、$[i]_q=1+q+\cdots+q^{i-1}$、$[n]_q!$、$q$-降冪、Gaussian二項係数を求める。係数型は静的・動的modint。動的modintの法はこのオブジェクトの使用中に固定する。

- $q\ne0,1$ のとき、$d=\operatorname{ord}(q)$ とすると $[d]_q=0$ なので、$n\ge d$ の $q$-階乗は0。$q$-二項係数は $q$-Lucasで $d$ 進の下位桁と通常の二項係数に分け、通常の二項係数には $p$ 進のLucasを適用する。
- $q=1$ は通常の整数・階乗・二項係数。$q=0$ は正の $i$ に対して $[i]_q=1$ で、範囲内の二項係数と降冪は1。空積は1。`order()` は $q\ne0$ の乗法位数を返し、$q=0$ では0。
- `nCk(n,k)` と `nPk(n,k)` は $k<0$ または $k>n\ge0$ で0を返す。`<true>` は $q\ne0$ で負の上側添字を許す。逆元のAPIには該当する値が非零であることが必要。

## インターフェース

```cpp
#include "library/math/modint_q_factorials.hpp"

template<class T> class modint_q_factorials;
explicit modint_q_factorials(T q, long long n = 0);
std::uint64_t order() const;
void precompute(long long n);
T q_integer(long long n) const;
T mint_inv(long long n);
T mint_factorial(long long n);
T mint_inv_factorial(long long n);
T inv(long long n);
T fact(long long n);
T ifact(long long n);
template<bool admit_negative = false> T nCk(long long n, long long k);
template<bool admit_negative = false> T nPk(long long n, long long k);
template<bool admit_negative = false> T nCk_inv(long long n, long long k);
template<bool admit_negative = false> T nPk_inv(long long n, long long k);
```

## 計算量

位数の計算には $p-1$ の素因数分解を使う。$q\ne0,1$ で `precompute(N)` を呼ぶと $\min(N,d-1)$ までの $q$-階乗と $\min(\lfloor N/d\rfloor,p-1)$ までの通常の階乗を作る。時間と領域は表の長さに比例し、各拡張で定数回の逆元計算を行う。$q=1$ では $\min(N,p-1)$ までの通常の階乗だけを作る。

表に収まる値の階乗・降冪・逆元は $O(1)$、二項係数は利用するLucasの桁数に比例する。表の自動拡張は1,000万を上限とし、表外の桁には乗積を使う。二項係数の乗積は下側の桁とその補数の小さい方を使い、通常の二項係数では上側の桁が $p-1$ に近い場合も反転する。負の上側添字では補正用の冪乗が加わる。

## 依存関係

- [internal_math.hpp](internal_math.md)
- [modint_traits.hpp](modint_traits.md)
- [prime_factorization.hpp](prime_factorization.md)

## Verify・利用例

- [`library_checker/q_binomial_coefficient_prime_mod`](../../../verify/library_checker/enumerative_combinatorics/q_binomial_coefficient_prime_mod.cpp)
- [`unit/modular_combinatorics`](../../../verify/unit/modular_combinatorics.cpp)
- [q-Lucasの根での評価](https://www.combinatorics.org/ojs/index.php/eljc/article/download/v19i3p1/pdf/)
