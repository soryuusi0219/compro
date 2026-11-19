# binomial_coefficient

[索引](../README.md) / [実装](../../../library/math/binomial_coefficient.hpp)

## 機能・前提条件

固定した正の `int` の法 $M$ で $\binom nk$ を計算する。法を素冪に分け、素因子を除いた階乗とその逆元で各剰余を求め、構築時に計算したCRT係数で復元する。

$n\ge0$ が前提。$k<0$、$k>n$、$M=1$ では0を返す。`table_limit` 以下の素冪に対して前計算を行う。上限を超える素冪は、クエリで使う積を計算する。

## インターフェース

```cpp
#include "library/math/binomial_coefficient.hpp"

class BinomialCoefficient;
explicit BinomialCoefficient(int mod, std::uint32_t table_limit = 1'000'000);
std::uint32_t mod() const;
std::uint32_t nCk(long long n, long long k) const;
```

## 計算量

法の素因数分解に加え、前計算を行う素冪の合計を $S$ として $O(S)$ 時間・領域。前計算済みの $p^e$ に対するクエリは $O(1+\log_p(n+1))$ 時間、CRTは素因子数に比例する。

表を省いた素数では通常のLucasを使い、各桁は乗積で計算する。表を省いた $p^e$（$e\ge2$）では短い側の項別積と、必要な剰余位置までの単元階乗の走査を切り替える。前者は $O(\min(k,n-k)+\log_p(n+1)+\log(p^e))$ 時間、後者は $O(p^e+L\log(L+1)+\log(p^e))$ 時間・$O(L)$ 領域で、$L=1+\log_p(n+1)$。どちらも積をまとめてから逆元を1回求める。

## 依存関係

- [internal_math.hpp](internal_math.md)
- [prime_factorization.hpp](prime_factorization.md)

## Verify・利用例

- [`library_checker/binomial_coefficient`](../../../verify/library_checker/enumerative_combinatorics/binomial_coefficient.cpp)
- [`unit/modular_combinatorics`](../../../verify/unit/modular_combinatorics.cpp)
- [素冪法の二項係数](https://www.cecm.sfu.ca/organics/papers/granville/paper/binomial/html/node2.html)
