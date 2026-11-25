# prime_field_tables

[索引](../README.md) / [実装](../../../library/math/prime_field_tables.hpp)

## 機能・前提条件

固定した `int` の素数 $p$ について、逆元、原始根を底とする離散対数、任意の底の冪乗を前計算後に $O(1)$ で求める。引数は $0\le x<p$ の剰余を渡す。`inv(x)` と `log(x)` は $x>0$ が前提。`log` の値は $[0,p-1)$ の代表元。

`pow(x,n)` は負の指数も扱い、$0^0=1$ とする。$x=0$ には非負の指数を渡す。`with_logarithms=false` で構築すると逆元用の表だけを作る。

## インターフェース

```cpp
#include "library/math/prime_field_tables.hpp"

class PrimeFieldTables;
explicit PrimeFieldTables(int modulus, bool with_logarithms = true);
std::uint32_t mod() const;
std::uint32_t generator() const;
std::uint32_t inv(std::uint32_t x) const;
std::uint32_t log(std::uint32_t x) const;
std::uint32_t pow(std::uint32_t x, long long n) const;
```

## 計算量

すべてのクエリは最悪 $O(1)$ 時間。$p\le2^{16}$ では全剰余を列挙して $O(p)$ 時間・領域で構築する。大きな法ではFarey列の区間で $x$ を小さい剰余の比に置き換える。逆元用の構築は $O(p^{2/3})$ 時間・領域。冪乗には原始根のべきの平方分割を用いる。

離散対数では $p-1$ の素因数分解を行い、$p-1=\prod \ell^e$ の各素冪について、$Q=\pi(\lfloor\sqrt p\rfloor)$ 個の素数にPohlig–Hellmanの桁ごとの処理を行う。素数位数 $\ell$ のBSGSは幅 $W=\min(\ell,\lceil\sqrt{\ell\max(1,Q)}\rceil)$ を共有する。各素冪の構築時間はハッシュ表を用いる期待値で $O(W+Qe(\log p+\ell/W))$、一時領域は $O(W)$。それより大きい引数の対数表は商と余りの関係から線形時間で拡張する。

## 依存関係

- [hash_table.hpp](../data_structure/hash_table.md)
- [internal_math.hpp](internal/internal_math.md)
- [linear_sieve.hpp](linear_sieve.md)
- [prime_factorization.hpp](prime_factorization.md)
- [internal_bit.hpp](internal/internal_bit.md)

## Verify・利用例

- [`unit/modular_combinatorics`](../../../verify/unit/modular_combinatorics.cpp)
- [`library_checker/discrete_logarithm_fixed_mod`](../../../verify/library_checker/number_theory/discrete_logarithm_fixed_mod.cpp) は入力の原始根 $g$ に合わせて $\log(a)/\log(g)\bmod(p-1)$ で底を変換する。
- [O(1) mod inv, mod pow](https://maspypy.com/o1-mod-inv-mod-pow)
