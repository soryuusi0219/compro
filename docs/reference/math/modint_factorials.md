# modint_factorials

[索引](../README.md) / [実装](../../../library/math/modint_factorials.hpp)

## 機能・前提条件

modintの逆元・階乗・逆階乗、順列・組合せ・Catalan・格子路。表を必要に応じ拡張する。必要な整数が可逆であること。

## インターフェース

```cpp
#include "library/math/modint_factorials.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
class modint_factorials;
modint_factorials() = default;
modint_factorials(int n);
void precompute(int n);
T mint_inv(int x);
T mint_factorial(int x);
T mint_inv_factorial(int x);
inline T inv(int x);
inline T fact(int x);
inline T ifact(int x);
template<bool admit_negative = false>
T nPk(int n, int k);
template<bool admit_negative = false>
T nPk_inv(int n, int k);
template<bool admit_negative = false>
T nCk(int n, int k);
template<bool admit_negative = false>
T nCk_inv(int n, int k);
T Catalan(int n);
T Catalan_convolution(int n, int k);
T Path_lower(int n, int m, int k);
T Path_between(int n, int m, int l, int k);
T Narayana(int n, int k);
```

## 計算量

最大参照Nまで合計 O(N) 前計算・空間。表を使う標準問い合わせ O(1)、Path_between等の和は項数に依存。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/binomial_coefficient_prime_mod`](../../../verify/library_checker/enumerative_combinatorics/binomial_coefficient_prime_mod.cpp)
