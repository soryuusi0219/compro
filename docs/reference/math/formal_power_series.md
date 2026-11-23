# formal_power_series

[索引](../README.md) / [実装](../../../library/math/formal_power_series.hpp)

## 機能・前提条件

modint向け形式的冪級数。四則演算、逆数、log/exp、冪、平方根、評価・補間、合成・逆合成、power projection、多項式の累積和、線形漸化式の復元・外挿など。定数項や可逆性の条件を各操作で満たすこと。

係数は定数項から順に格納します。`inv` は定数項が可逆、`log` は定数項1、`exp` は定数項0を要求します。微積分・階乗を使う操作では必要な整数の逆元が存在することも必要です。`sqrt` は入力の定数項が1で、必要な精度が2以上なら2が可逆であることを前提とします。`sqrt_general` は先頭の非零係数の平方根を `modsqrt` で選び、平方根が存在しなければ空の級数を返します。

## インターフェース

```cpp
#include "library/math/formal_power_series.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class Factorials = modint_factorials<T>>
struct FormalPowerSeries : vector<T>;
using vector<T>::vector;
using FPS = FormalPowerSeries;
FormalPowerSeries(vector<T>&& v);
template<class U> FormalPowerSeries(const vector<U>& v);
FormalPowerSeries(T x);
friend FPS& operator+=(FPS& x, const FPS& y);
friend FPS& operator-=(FPS& x, const FPS& y);
friend FPS& operator+=(FPS& x, const T& y);
friend FPS& operator-=(FPS& x, const T& y);
friend FPS& operator*=(FPS& x, const T& y);
friend FPS& operator/=(FPS& x, const T& y);
friend FPS operator+(FPS x, const FPS& y);
friend FPS operator-(FPS x, const FPS& y);
friend FPS operator+(FPS x, const T& y);
friend FPS operator-(FPS x, const T& y);
friend FPS operator*(FPS x, const T& y);
friend FPS operator/(FPS x, const T& y);
FPS operator-() const;
friend FPS operator*(const FPS& x, const FPS& y);
friend FPS& operator*=(FPS& x, const FPS& y);
friend FPS operator/(FPS x, const FPS& y);
friend FPS& operator/=(FPS& x, const FPS& y);
friend FPS operator%(const FPS& x, const FPS& y);
friend FPS& operator%=(FPS& x, const FPS& y);
FPS inv(int N = -1) const;
friend FPS& operator>>=(FPS& x, int k);
friend FPS& operator<<=(FPS& x, int k);
friend FPS operator>>(FPS x, int k);
friend FPS operator<<(FPS x, int k);
T eval(T x) const;
FPS& shrink();
FPS& reversed();
FPS rev() const;
FPS& pred(int k);
FPS pre(int k) const;
FPS dot(const FPS& other) const;
FPS& diffed();
FPS& integraled();
FPS diff() const;
FPS integral() const;
FPS log(int N = -1) const;
FPS exp(int N = -1) const;
FPS pow(long long int k, int N = -1) const;
FPS sqrt(int N = -1) const;
FPS sqrt_general(int N = -1) const;
pair<FPS,FPS> div(FPS g) const;
vector<T> mult_eval(const vector<T>& p) const;
vector<T> to_newton(const vector<T>& p) const;
vector<T> geo_eval(T a, T r, int m);
static FPS interpolation(const vector<T>& x, const vector<T>& y);
static FPS prod(vector<FPS> F);
static FPS geo_interpolation(const vector<T>& y, T a, T r);
static pair<FPS,FPS> quotient_sum(vector<pair<FPS,FPS>> QR);
FPS Taylor_shift(T a) const;
static vector<T> shift_sampling(vector<T> f, T c, int m = -1);
FPS times_sparse(const vector<pair<int,T>>& x, int deg = -1) const;
static FPS inv_sparse(const vector<pair<int,T>>& x, int deg);
FPS div_sparse(const vector<pair<int,T>>& x, int deg = -1);
static FPS log_sparse(const vector<pair<int,T>>& x, int deg);
static FPS exp_sparse(const vector<pair<int,T>>& x, int deg);
static FPS _pow_sparse(const vector<pair<int,T>>& x, T m, int deg);
static FPS pow_sparse(vector<pair<int,T>> x, long long m, int deg);
static FPS sqrt_sparse(const vector<pair<int,T>>& x,  int deg);
static FPS sqrt_general_sparse(vector<pair<int,T>> x, int deg);
vector<T> NewtonBasis(vector<T> p) const;
static T Bostan_Mori(FPS F, FPS G, long long int k);
FPS Mobius_Transform(T a, T b, T c, T d) const;
T prefix_product(long long k) const;
FPS prefix_sum() const;
T sum_of_exponential_times_polynomial(T r, long long n) const;
T sum_of_exponential_times_polynomial_limit(T r) const;
static T sum_of_exponential_times_polynomial(vector<T> values, T r, long long n);
static T sum_of_exponential_times_polynomial_limit(const vector<T>& values, T r);
static FPS find_linear_recurrence(FPS samples);
static T kth_term_of_linear_recurrence(const FPS& initial, const FPS& recurrence, long long k);
static FPS linear_recurrence_terms(const FPS& initial, const FPS& recurrence, long long k, int count);
static T kth_term_of_linear_recurrence(const FPS& samples, long long k);
static FPS linear_recurrence_terms(const FPS& samples, long long k, int count);
struct Mat;
Mat() = default;
Mat(const FPS& a_, const FPS& b_, const FPS& c_, const FPS& d_);
Mat& operator*=(const Mat& r);
static Mat I();
Mat operator*(const Mat& r) const;
pair<FPS, FPS> app(const pair<FPS, FPS>& p) const;
static void Naive_GCD(Mat& M, pair<FPS, FPS>& p, T& res, int pre);
static Mat Half_GCD(pair<FPS, FPS> p, T& res, int pre = 0);
static pair<Mat, T> GCD_Mat(const FPS& f, const FPS& g);
static FPS PolyGCD(const FPS& f, const FPS& g);
FPS gcd(const FPS& g) const;
static pair<bool, FPS> PolyInv(const FPS& f, const FPS& g);
pair<bool, FPS> inv_mod(const FPS& g) const;
static T Resultant(const FPS& f, const FPS& g);
T resultant(const FPS& g) const;
vector<T> FindRoot() const;
static FPS PowerProjection(FPS F, FPS G, int N, int M);
static FPS Compositional_inverse(FPS F, int m = -1);
static FPS Composition(FPS F, FPS G, int m = -1);
FPS composition(const FPS& g, int d = -1) const;
FPS compositional_inverse(int d = -1) const;
template <typename T, bool ntt_friendly = is_ntt_friendly_v<T>>
struct static_composition;
static_composition(const FormalPowerSeries<T>& G);
static_composition(const FormalPowerSeries<T>& G, int m_);
FormalPowerSeries<T> composition(FormalPowerSeries<T> F) const;
template <typename T, int b = 4, bool ntt_friendly = is_ntt_friendly_v<T>>
struct online_composition;
online_composition(FormalPowerSeries<T> G, int n_);
T query(T x);
T get(int i) const;
template <typename T, class Factorials = modint_factorials<T>>
vector<T> CompositionalPowerProjection(FormalPowerSeries<T> F, int n = -1, int m = -1, FormalPowerSeries<T> C1 = {}, FormalPowerSeries<T> C2 = {}, FormalPowerSeries<T> D = {});
```

- `f.composition(g,m)` は有限多項式 `f(g(x))` の先頭 `m` 係数を返す。内側の定数項が0以外でも、渡した外側の全係数を使う。省略時の `m` は両入力長の最大値。
- `f.compositional_inverse(m)` は `f(g(x)) = x mod x^m` を満たす `g` を求める。精度2以上では `f[0] == 0` と `f[1]` が可逆であること。
- `PowerProjection(f,g,n,m)` は `[x^(n-1)] g(x) f(x)^i` を `0 <= i < m` に対して返す。取り出す次数 `n-1` と出力数 `m` は独立。
- `static_composition<T>(g,m)` は同じ内側の級数に対して複数の外側の級数を合成する。精度省略時は `g.size()`。
- `online_composition<T>(g,n)` は `g[0] == 0` を前提に、`query(f[i])` で外側の係数を低次から受け取り、合成結果の `i` 次係数を返す。`get(i)` は入力済みの係数だけで計算した値。次の添字より先を参照する場合は追加計算を行う。
- `CompositionalPowerProjection` は `C2 o F^(o i) o C1` に `D` を掛けた `n-1` 次係数を返す。`F` と `C1` の定数項は0。`C1`、`C2` の空入力は恒等写像、`D` の空入力は1。線形化や放物型の正規形が使える場合はまとめて計算し、それ以外は反復合成を使う。

合成系はNTTに適さない静的modintや動的modintでも使える。逆合成で必要な整数が可逆な素数法ではLagrange反転を使い、それ以外ではNewton法を使う。CRTの復元範囲を超える法では直接積を使う。

- `f.prefix_sum()` は `g(0) = 0`、`g(x+1) - g(x) = f(x)` を満たす多項式を係数列で返す。非負整数 `n` に対し $g(n)=\sum_{i=0}^{n-1}f(i)$。長さ $N$ の入力に対して長さ $N+1$ を返し、空入力は空を返す。係数型で $1,\ldots,N$ が可逆であること。Bernoulli数は $B_1=-1/2$ の規約を使う。
- `f.sum_of_exponential_times_polynomial(r,n)` は `n >= 0` に対し $\sum_{i=0}^{n-1} f(i)r^i$ を返す。`f.sum_of_exponential_times_polynomial_limit(r)` は母関数 $\sum_{i\ge0} f(i)x^i$ を有理式にして $x=r$ で評価する。無限和と `r != 1` の有限和には $1-r$ と $1,\ldots,\deg f$ の逆元を使う。`r == 1` の有限和で級数を使う場合は $\deg f+1$ までの逆元が必要。`r == 0` は定数項を読む。
- 重み付き和の静的オーバーロードは $\deg f\le d$ として `values = {f(0),...,f(d)}` を受け取る。補間と外挿には $1,\ldots,d+1$ の逆元が必要。`r` が0でも1でもない有限和の外挿では `r` の逆元も使う。有限和で `n <= values.size()` の場合は入力済みの値から直接計算する。
- `find_linear_recurrence(samples)` は先頭から渡した有限数列に合う最短の漸化式 `c` を返す。`d = c.size()` として、`i >= d` で `a[i] = c[0]*a[i-1] + ... + c[d-1]*a[i-d]`。末尾の0は次数に含める。係数は体を前提とし、空列や全項0の列では次数0を返す。
- `kth_term_of_linear_recurrence(initial,c,k)` は0始まりの `a[k]` を返す。`linear_recurrence_terms(initial,c,k,count)` は `a[k]` から連続する `count` 項を返す。`initial` の長さは `c.size()`、`k` と `count` は非負。次数0の漸化式は全項0を表す。
- 漸化式を省略したオーバーロードでは、必要に応じて `samples` から最短の漸化式を復元して外挿する。有限の初項列から複数の延長が可能な場合は、復元した最短漸化式に従う。

## 計算量

畳み込み時間を $M(N)$ とする。標準的な $M(N)=O(N\log N)$ の場合、逆数・指数・対数・平方根は $O(M(N))$ 時間。NTT版の指数は逆数をNewton段間で引き継ぐ。多点評価は $O(M(N)\log N)$ 時間。

同程度のサイズ $N$ での合成・逆合成・power projectionは $O(M(N)\log N)$ 時間。合成の事前計算は $O(N\log N)$ 領域を使い、固定した内側の級数に再利用できる。係数型と各パラメータによる詳細は[内部実装](internal/internal_fps_composition.md)を参照。

`CompositionalPowerProjection` が反復合成を使う場合、精度 $N$、出力数 $m$ に対して $O(mM(N)\log N)$ 時間。

- `prefix_sum` は級数の逆数でBernoulli数の指数型母関数を作り、畳み込みで累積和を求める。係数数 $N$ に対して $O(M(N))$ 時間、$O(N)$ 作業領域。
- 係数からの重み付き和は、重み付き冪和の指数型母関数を使い、係数数 $N$ に対して $O(M(N)+\log(n+2))$ 時間。連続点での値を渡す場合は補間を用い、$O(N+\log(n+2))$ 時間。どちらも $O(N)$ 領域。無限和では $n$ に依存する項が外れる。
- `find_linear_recurrence` はHalf GCDでPadé近似を求め、初項数 $N$ に対して $O(M(N)\log(N+2))$ 時間。
- 次数 $d$ が既知の場合、単項取得はBostan–Moriで $O(M(d+1)\log(k+2))$ 時間。連続 $m$ 項は逆数の係数窓から $x^k$ を特性多項式で剰余し、$O(M(d+1)\log(k+2)+M(d+m+1))$ 時間、$O(d\log(k+2)+m)$ 作業領域で求める。漸化式を省略した場合は、必要に応じて復元の計算量が加わる。[Bostan–Mori](https://arxiv.org/abs/2008.08822) / [逆数の連続係数の計算](https://noshi91.hatenablog.com/entry/2023/06/04/233447)

## 依存関係

- [convolution.hpp](convolution.md)
- [internal_fps_composition.hpp](internal/internal_fps_composition.md)
- [internal_math.hpp](internal/internal_math.md)
- [modint_traits.hpp](internal/modint_traits.md)
- [modint_factorials.hpp](modint_factorials.md)
- [ntt.hpp](ntt.md)
- [convolution_mod.hpp](convolution_mod.md)
- [convolution_semi_online.hpp](convolution_semi_online.md)
- [convolution_online.hpp](convolution_online.md)
- [internal_bit.hpp](internal/internal_bit.md)

## Verify・利用例

- [`unit/formal_power_series`](../../../verify/unit/formal_power_series.cpp)
- [`unit/combinatorics`](../../../verify/unit/combinatorics.cpp)
- [`library_checker/sum_of_exponential_times_polynomial`](../../../verify/library_checker/other/sum_of_exponential_times_polynomial.cpp)
- [`library_checker/sum_of_exponential_times_polynomial_limit`](../../../verify/library_checker/other/sum_of_exponential_times_polynomial_limit.cpp)
- [`library_checker/composition_of_formal_power_series_crt`](../../../verify/library_checker/polynomial/composition_of_formal_power_series_crt.cpp)
- [`library_checker/compositional_inverse_of_formal_power_series_crt`](../../../verify/library_checker/polynomial/compositional_inverse_of_formal_power_series_crt.cpp)
- [`library_checker/factorial`](../../../verify/library_checker/enumerative_combinatorics/factorial.cpp)
- [`library_checker/kth_term_of_linearly_recurrent_sequence`](../../../verify/library_checker/other/kth_term_of_linearly_recurrent_sequence.cpp)
- [`library_checker/find_linear_recurrence`](../../../verify/library_checker/other/find_linear_recurrence.cpp)
- [`library_checker/consecutive_terms_of_linear_recurrent_sequence`](../../../verify/library_checker/other/consecutive_terms_of_linear_recurrent_sequence.cpp)
- [`library_checker/prefix_sum_of_polynomial`](../../../verify/library_checker/polynomial/prefix_sum_of_polynomial.cpp)
- [`library_checker/bell_number`](../../../verify/library_checker/enumerative_combinatorics/bell_number.cpp)
- [`library_checker/partition_function`](../../../verify/library_checker/enumerative_combinatorics/partition_function.cpp)
- [`library_checker/sharp_p_subset_sum`](../../../verify/library_checker/enumerative_combinatorics/sharp_p_subset_sum.cpp)
- [`library_checker/bernoulli_number`](../../../verify/library_checker/number_theory/bernoulli_number.cpp)
- [`library_checker/composition_of_formal_power_series`](../../../verify/library_checker/polynomial/composition_of_formal_power_series.cpp)
- [`library_checker/composition_of_formal_power_series_large`](../../../verify/library_checker/polynomial/composition_of_formal_power_series.cpp)
- [`library_checker/compositional_inverse_of_formal_power_series`](../../../verify/library_checker/polynomial/compositional_inverse_of_formal_power_series.cpp)
- [`library_checker/compositional_inverse_of_formal_power_series_large`](../../../verify/library_checker/polynomial/compositional_inverse_of_formal_power_series.cpp)
- [`library_checker/conversion_from_monomial_basis_to_newton_basis`](../../../verify/library_checker/polynomial/conversion_from_monomial_basis_to_newton_basis.cpp)
- [`library_checker/division_of_polynomials`](../../../verify/library_checker/polynomial/division_of_polynomials.cpp)
- [`library_checker/exp_of_formal_power_series`](../../../verify/library_checker/polynomial/exp_of_formal_power_series.cpp)
- [`library_checker/exp_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/exp_of_formal_power_series_sparse.cpp)
- [`library_checker/inv_of_formal_power_series`](../../../verify/library_checker/polynomial/inv_of_formal_power_series.cpp)
- [`library_checker/inv_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/inv_of_formal_power_series_sparse.cpp)
- [`library_checker/inv_of_polynomials`](../../../verify/library_checker/polynomial/inv_of_polynomials.cpp)
- [`library_checker/log_of_formal_power_series`](../../../verify/library_checker/polynomial/log_of_formal_power_series.cpp)
- [`library_checker/log_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/log_of_formal_power_series_sparse.cpp)
- [`library_checker/multipoint_evaluation`](../../../verify/library_checker/polynomial/multipoint_evaluation.cpp)
- [`library_checker/multipoint_evaluation_on_geometric_sequence`](../../../verify/library_checker/polynomial/multipoint_evaluation_on_geometric_sequence.cpp)
- [`library_checker/polynomial_interpolation`](../../../verify/library_checker/polynomial/polynomial_interpolation.cpp)
- [`library_checker/polynomial_interpolation_on_geometric_sequence`](../../../verify/library_checker/polynomial/polynomial_interpolation_on_geometric_sequence.cpp)
- [`library_checker/polynomial_root_finding`](../../../verify/library_checker/polynomial/polynomial_root_finding.cpp)
- [`library_checker/polynomial_taylor_shift`](../../../verify/library_checker/polynomial/polynomial_taylor_shift.cpp)
- [`library_checker/pow_of_formal_power_series`](../../../verify/library_checker/polynomial/pow_of_formal_power_series.cpp)
- [`library_checker/pow_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/pow_of_formal_power_series_sparse.cpp)
- [`library_checker/product_of_polynomial_sequence`](../../../verify/library_checker/polynomial/product_of_polynomial_sequence.cpp)
- [`library_checker/shift_of_sampling_points_of_polynomial`](../../../verify/library_checker/polynomial/shift_of_sampling_points_of_polynomial.cpp)
- [`library_checker/sqrt_of_formal_power_series`](../../../verify/library_checker/polynomial/sqrt_of_formal_power_series.cpp)
- [`library_checker/sqrt_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/sqrt_of_formal_power_series_sparse.cpp)
