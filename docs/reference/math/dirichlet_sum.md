# dirichlet_sum

[索引](../README.md) / [実装](../../../library/math/dirichlet_sum.hpp)

## 機能・前提条件

数論関数 $f$ の係数と累積和を保持し、Dirichlet畳み込み・除算・非負整数冪を行う。
`init(n,k)` で $N=n$、$K=\max(k,\lceil\sqrt N\rceil)$、$L=\lceil N/K\rceil$ を設定する。$n\ge 1$、$k\ge 0$ とする。
`al[i-1]` は $f(i)$（$1\le i\le K$）、`au[j-1]` は `eval_points[j-1]`（$\lfloor N/j\rfloor$）以下の累積和。
重なる点では両方の表の値が一致するように作る。

`N`、`K`、`L`、`eval_points` と素数表は `(T,id)` ごとに共有する。同じ型とIDで異なる設定に再初期化した後は、以前の設定で作った値を使わない。
`is_mul=true` は $f(1)=1$ を満たす乗法的関数を渡す場合に指定する。除算では分母の $f(1)$ が係数型で可逆であること。

## インターフェース

```cpp
#include "library/math/dirichlet_sum.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, int id = -1>
struct Dirichlet_Sum;
static void init(long long n, long long k);
Dirichlet_Sum();
Dirichlet_Sum(vector<T> _al, vector<T> _au, bool is_mul = false);
Dirichlet_Sum(vector<T> _al, T (*f)(long long), bool is_mul = false);
Dirichlet_Sum(T (*fl)(long long), T (*fu)(long long), bool is_mul = false);
Dirichlet_Sum operator+=(const Dirichlet_Sum& other);
Dirichlet_Sum operator+(const Dirichlet_Sum& other) const;
Dirichlet_Sum operator-=(const Dirichlet_Sum& other);
Dirichlet_Sum operator-(const Dirichlet_Sum& other) const;
Dirichlet_Sum operator*=(const T x);
Dirichlet_Sum operator*(const T x) const;
Dirichlet_Sum operator*(const Dirichlet_Sum& other) const;
Dirichlet_Sum operator*=(const Dirichlet_Sum& other);
Dirichlet_Sum operator/(const Dirichlet_Sum& other) const;
Dirichlet_Sum operator/=(const Dirichlet_Sum& other);
Dirichlet_Sum pow(long long k);
static Dirichlet_Sum e();
static Dirichlet_Sum zeta();
static Dirichlet_Sum zetak(int k);
static Dirichlet_Sum zetashift(int k);
inline T getN();
inline T getN() const;
inline T getNdivi(int i);
inline T getNdivi(int i) const;
inline vector<T> getupper();
inline vector<T> getupper() const;
inline pair<vector<T>,vector<T>> getAll();
inline pair<vector<T>,vector<T>> getAll() const;
static long long N, K, L;
static vector<long long> eval_points, primes, divisors;
```

- `e()` はDirichlet畳み込みの単位元、`zeta()` は全係数が1の関数。 `zetak(k)` は正整数 $k$ に対して完全 $k$ 乗数の指示関数、`zetashift(k)` は $1\le k\le 5$ の $f(i)=i^k$。
- `getN()` と `getNdivi(i)` は累積和を返す。`getupper()` と `getAll()` は表をコピーする。
- `pow(k)` は二分累乗法と、定数項を分離した打ち切り多項式の評価を使い分ける。後者も係数の割り算を使わない。

## 計算量

保持領域と積・除算の作業領域は $O(K+L)$。
積・除算の商集合側は $O(N^{2/3})$ 時間。約 $N^{2/3}/4$ までの商は、同じ累積和に反映される積をまとめて集計し、それより大きい商は双曲線法で求める。法が $2^{31}$ 未満の静的modintでは双曲線法の積和をまとめて剰余に戻し、積で繰り返し使う一部の整数除算を乗算に置き換える。

係数 $1\ldots K$ の計算は一般に $O(K\log(K+1))$ 時間。両入力が乗法的なら素数冪から $O(K)$ 時間で復元する。積の片方、または除算の分母が乗法的な場合は素数冪ごとの更新で $O(K\log\log(K+2))$ 時間。

## 依存関係

- [modint_traits.hpp](internal/modint_traits.md)
- [internal_bit.hpp](internal/internal_bit.md)

## Verify・利用例

- [`library_checker/dirichlet_convolution_and_prefix_sums`](../../../verify/library_checker/number_theory/dirichlet_convolution_and_prefix_sums.cpp)
- [`library_checker/dirichlet_inverse_and_prefix_sums`](../../../verify/library_checker/number_theory/dirichlet_inverse_and_prefix_sums.cpp)
- [`library_checker/sum_of_totient_function_dirichlet_sum`](../../../verify/library_checker/number_theory/sum_of_totient_function_dirichlet_sum.cpp)
- [`unit/dirichlet_sum`](../../../verify/unit/dirichlet_sum.cpp)
- [商集合を使うLibrary Checkerの公式解法](https://github.com/yosupo06/library-checker-problems/blob/73876f1cbcf082d2b22722924b924d3d65a2f00e/number_theory/dirichlet_inverse_and_prefix_sums/sol/correct.cpp)
