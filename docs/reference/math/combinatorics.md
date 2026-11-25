# combinatorics

[索引](../README.md) / [実装](../../../library/math/combinatorics.hpp)

## 機能・前提条件

Bernoulli数、Bell数、階乗、Montmort数（撹乱順列の個数）、分割数、符号付き第1種・第2種Stirling数、部分集合和の個数を求める。Montmort数は法を整数で渡し、それ以外の係数型 `T` はmodintを使う。

- `bernoulli_numbers(n)`、`bell_numbers(n)`、`partition_numbers(n)` は添字 $0$ から $n$ までを返す。Bernoulli数は $B_1=-1/2$ の規約を使う。$n\ge2$ のBernoulli数には $1,\ldots,2\lfloor n/2\rfloor+1$、Bell数には $1,\ldots,n$ の逆元が必要。分割数ではEulerの五角数定理を使う。
- `factorial(n)` は $n!$、`many_factorials(queries)` は入力と同じ順に各 $n!$ を返す。$n\ge0$ を渡し、$n\ge T::mod()$ なら結果は0。法が `int` に収まる素数で添字が大きい場合はブロック積を連続点評価する。素数法ではWilsonの定理で計算範囲を半分にする。
- `montmort_numbers(n,mod)` は $D_1,\ldots,D_n$ を返す。$n=0$ では空。法は正の64ビット符号なし整数で、合成数も使える。
- `stirling_first_kind(n)` と `stirling_second_kind(n)` は $k=0,\ldots,n$ の行を返す。第1種は降冪 $(x)_n=x(x-1)\cdots(x-n+1)$ の係数、第2種は $x^n=\sum_k S(n,k)(x)_k$ の係数。Taylor shiftに使う $1,\ldots,\lfloor n/2\rfloor$（第1種）または $1,\ldots,n$（第2種）が可逆であること。
- `stirling_first_kind_fixed_k(n,k)` と `stirling_second_kind_fixed_k(n,k)` は $0\le k\le n$ を前提に、添字 $i=k,\ldots,n$ の列を返す。FPSを使う場合は $1,\ldots,n-k+1$ が可逆であること。
- `sharp_p_subset_sum(values,max_sum)` は、添字集合の和が $s$ となる個数を $0\le s\le\texttt{max\_sum}$ の順に返す。空集合も含め、値が同じ要素は添字で区別する。入力値と `max_sum` は非負。0の要素は全係数を2倍し、`max_sum` より大きい要素は結果に影響しない。

## インターフェース

```cpp
#include "library/math/combinatorics.hpp" // namespace soryuusi

template<class T> std::vector<T> bernoulli_numbers(int n);
template<class T> std::vector<T> bell_numbers(int n);
template<class T> T factorial(long long n);
template<class T, std::integral Index> std::vector<T> many_factorials(const std::vector<Index>& queries);
std::vector<std::uint64_t> montmort_numbers(int n, std::uint64_t mod);
template<class T> std::vector<T> partition_numbers(int n);
template<class T> std::vector<T> sharp_p_subset_sum(const std::vector<int>& values, int max_sum);
template<class T> std::vector<T> stirling_first_kind(int n);
template<class T> std::vector<T> stirling_second_kind(int n);
template<class T> std::vector<T> stirling_first_kind_fixed_k(int n, int k);
template<class T> std::vector<T> stirling_second_kind_fixed_k(int n, int k);
```

## 計算量

$M(N)$ を長さ $N$ の畳み込み時間とする。Bernoulli数、Bell数、分割数、第1種Stirling数の行は $O(M(n+1))$ 時間。第2種の行は篩で素数だけを冪乗し、$O(M(n+1)+n+\pi(n)\log(n+1))$ 時間。固定 $k$ の列は残りの精度 $L=n-k+1$ で $O(M(L))$ 時間。いずれも使用する級数の精度に比例する領域を使う。Montmort数には時間と出力領域が $O(n)$ 必要。

階乗は、$Q$ を問い合わせ数、$U$ を計算する最大添字とすると、逐次積で $O(U+Q\log(Q+1))$ 時間。素数法では $U$ をWilsonの定理で縮める。ブロック幅を $B$ とした大きな素数法の経路では、$O(M(B)+M(U/B+1)+\min(U,QB)+Q\log(Q+1))$ 時間、$O(Q+B+U/B)$ 領域。単一問い合わせは $B$ を $\sqrt{U}$ の定数倍に選ぶ。

部分集合和では $T=\texttt{max\_sum}$、$L$ を採用した正の要素の総和と $T$ の小さい方とする。正の値の出現回数をまとめて対数を作り、FPSの指数で係数を戻す。$1$ から $L$ までの逆元が必要。入力数を $N$ とすると $O(N+T+L\log(L+1)+M(L+1))$ 時間、$O(T+1)$ 領域。

## 依存関係

- [formal_power_series.hpp](formal_power_series.md)
- [internal_fps_composition.hpp](internal/internal_fps_composition.md)
- [internal_math.hpp](internal/internal_math.md)
- [linear_sieve.hpp](linear_sieve.md)
- [modint_factorials.hpp](modint_factorials.md)
- [modint_traits.hpp](internal/modint_traits.md)
- [internal_bit.hpp](internal/internal_bit.md)

## Verify・利用例

- [`unit/combinatorics`](../../../verify/unit/combinatorics.cpp)
- [`library_checker/bernoulli_number`](../../../verify/library_checker/number_theory/bernoulli_number.cpp)
- [`library_checker/bell_number`](../../../verify/library_checker/enumerative_combinatorics/bell_number.cpp)
- [`library_checker/factorial`](../../../verify/library_checker/enumerative_combinatorics/factorial.cpp)
- [`library_checker/many_factorials`](../../../verify/library_checker/enumerative_combinatorics/many_factorials.cpp)
- [`library_checker/montmort_number_mod`](../../../verify/library_checker/enumerative_combinatorics/montmort_number_mod.cpp)
- [`library_checker/partition_function`](../../../verify/library_checker/enumerative_combinatorics/partition_function.cpp)
- [`library_checker/sharp_p_subset_sum`](../../../verify/library_checker/enumerative_combinatorics/sharp_p_subset_sum.cpp)
- [`library_checker/stirling_number_of_the_first_kind`](../../../verify/library_checker/enumerative_combinatorics/stirling_number_of_the_first_kind.cpp)
- [`library_checker/stirling_number_of_the_second_kind`](../../../verify/library_checker/enumerative_combinatorics/stirling_number_of_the_second_kind.cpp)
- [`library_checker/stirling_number_of_the_first_kind_fixed_k`](../../../verify/library_checker/enumerative_combinatorics/stirling_number_of_the_first_kind_fixed_k.cpp)
- [`library_checker/stirling_number_of_the_second_kind_fixed_k`](../../../verify/library_checker/enumerative_combinatorics/stirling_number_of_the_second_kind_fixed_k.cpp)
