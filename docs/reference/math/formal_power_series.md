# formal_power_series

[索引](../README.md) / [実装](../../../library/math/formal_power_series.hpp)

## 機能・前提条件

modint向け形式的冪級数。四則演算、逆数、log/exp、冪、評価・補間など。定数項や可逆性の条件を各操作で満たすこと。

係数は定数項から順に格納します。`inv` は定数項が可逆、`log` は定数項1、`exp` は定数項0を要求します。微積分・階乗を使う操作では必要な整数の逆元が存在することも必要です。

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
template <typename T>
struct static_composition<T, true>;
static_composition(const FormalPowerSeries<T>& G);
static_composition(const FormalPowerSeries<T>& G, int m_);
FormalPowerSeries<T> composition(FormalPowerSeries<T> F) const;
template <typename T, int b = 4, bool ntt_friendly = is_ntt_friendly_v<T>>
struct online_composition;
online_composition(FormalPowerSeries<T> G, int n_);
T query(T x);
inline T get(int i) const;
template<typename T, int b>
struct online_composition<T, b, true>;
online_composition(FormalPowerSeries<T> G, int n_);
T query(T x);
inline T get(int i);
template <typename T, class Factorials = modint_factorials<T>>
vector<T> CompositionalPowerProjection(FormalPowerSeries<T> F, int n = -1, int m = -1, FormalPowerSeries<T> C1 = {}, FormalPowerSeries<T> C2 = {}, FormalPowerSeries<T> D = {});
```

## 計算量

畳み込みを M(N) として基本積 O(M(N))、Newton系 O(M(N) log N) を上界とする。多点処理 O(M(N) log N) 程度。空間は操作依存。

## 依存関係

- [convolution.hpp](convolution.md)
- [modint_traits.hpp](modint_traits.md)
- [modint_factorials.hpp](modint_factorials.md)
- [convolution_mod.hpp](convolution_mod.md)
- [convolution_semi_online.hpp](convolution_semi_online.md)
- [convolution_online.hpp](convolution_online.md)

## Verify・利用例

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
