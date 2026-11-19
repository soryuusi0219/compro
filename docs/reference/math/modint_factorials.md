# modint_factorials

[索引](../README.md) / [実装](../../../library/math/modint_factorials.hpp)

## 機能・前提条件

modintの逆元・階乗・逆階乗、順列・組合せ・Catalan・格子路。必要な整数が可逆であること。

表は法ごとに共有し、必要に応じて最大の添字まで拡張する。同じ法を使う別のオブジェクトや静的・動的modint型からも同じ表を参照する。動的modintは呼び出し時の法を使うため、法を戻すと以前の表を再利用する。表はプログラム終了まで保持する。複数スレッドで使う場合、表の初期化・拡張や法の切り替えは、読み取りも含めて呼び出し側で同期する。

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

法ごとの最大参照を $N_p$ として合計 $O(\sum_p(N_p+1))$ の前計算時間・領域。現在の表を使う標準問い合わせは $O(1)$。型が初めて表を参照するときや、動的modintの法を切り替えたときは、保持している法の数を $D$ として $O(\log(D+1))$ の検索が加わる。`Path_between` 等の和は項数に依存する。

## 依存関係

- [internal_math.hpp](internal_math.md)
- [modint_traits.hpp](modint_traits.md)

## Verify・利用例

- [`library_checker/binomial_coefficient_prime_mod`](../../../verify/library_checker/enumerative_combinatorics/binomial_coefficient_prime_mod.cpp)
- [`unit/combinatorics`](../../../verify/unit/combinatorics.cpp)
