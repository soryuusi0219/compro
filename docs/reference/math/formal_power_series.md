# formal_power_series

[索引](../README.md) / [実装](../../../library/math/formal_power_series.hpp)

## 機能・前提条件

modintを係数に持つ形式的冪級数と多項式。係数は定数項から順に並べ、`f[i]` が $x^i$ の係数を表す。末尾の0も格納されるため、`size()` は格納した係数数。有限多項式として次数を求める場合は `shrink()` で末尾の0を除く。空列と全係数0の列はいずれも零多項式を表す。

`inv`、`log`、`exp`、`pow`、`sqrt`、`sqrt_general` の精度 `N` は返す係数数。省略時は `f.size()` で、入力長を超える係数は0として扱う。長さを明示するときは非負の値を渡す。合成系の省略時の精度は各APIの欄に記載する。

- 係数型 `T` は操作に応じて、整数からの構築、環演算、`inv()`、`pow(k)`、`T::mod()`、`T::raw()`、`val()` が使えるmodintを想定する。例えば `soryuusi::modint998244353`、`modint1000000007`、動的modintを使える。動的modintの法は係数を作る前に設定し、係数や事前計算を使い終えるまで固定する。
- 積は [NTT適性の判定](internal/modint_traits.md) に従い、通常のNTTまたは3素数NTTとCRTを選ぶ。CRTの復元範囲を超える場合は直接積に切り替える。NTTを使う計算は変換長が対応範囲に収まること。
- 逆元を使う操作では、対象の係数や整数が可逆であること。例えば素数法 $p$ で $1,\ldots,N-1$ の逆元を使う操作は $N\le p$ が目安。既定の階乗表は必要な整数が可逆な範囲で使う。多項式GCD・漸化式復元は体を前提とする。
- 第2テンプレート引数 `Factorials` は階乗表の型。使う操作に応じて `mint_inv(i)`、`mint_factorial(i)`、`mint_inv_factorial(i)`、`nCk(n,k)` を持たせる。既定の表は法ごとに共有され、明示的な前計算は任意。重み付き和は専用の階乗・逆階乗計算を使う。[modint](modint.md) / [modint_factorials](modint_factorials.md)

## インターフェース

以下の宣言では `FPS` は `FormalPowerSeries<T, Factorials>` の別名。表中の $n$ は入力の係数数、$N$ は出力精度、$M(s)$ は長さ $O(s)$ の畳み込みに必要な時間。

### 構築・基本演算

```cpp
#include "library/math/formal_power_series.hpp"

template<class T, class Factorials = modint_factorials<T>>
struct FormalPowerSeries : std::vector<T>;

using FPS = FormalPowerSeries;
inline static Factorials factorials{};
using std::vector<T>::vector;
FormalPowerSeries(std::vector<T>&& v);
template<class U> FormalPowerSeries(const std::vector<U>& v);
FormalPowerSeries(T x);
```

`FPS{a,b,c}` は $a+bx+cx^2$、`FPS(n)` は $n$ 個の0、`FPS(T(a))` は定数 $a$ を格納する。公開継承した `std::vector` の添字アクセスやイテレータも使える。

| 形式 | 動作 |
| --- | --- |
| `f + g`、`f - g`、`-f` | 係数ごとの加減算・符号反転。加減算は両辺の長さの最大値を返す。 |
| `f + a`、`f - a`、`f * a`、`f / a` | `a` は係数型。加減算は定数項、乗除算は全係数に作用する。除算は `a.inv()` を使う。 |
| `f * g` | 完全な多項式積。非空の入力長を $n,m$ とすると長さは $n+m-1$。片方が空なら空を返す。 |
| `f / g` | `f * g.inv(f.size())` の先頭 `f.size()` 係数。正の精度では `g[0]` が可逆であること。 |
| `f.div(g)`、`f % g` | 多項式除算。`div` は `std::pair<FPS,FPS>` の商・余り、`%` は余り。末尾の0を除いた `g` が非零で、最高次係数が可逆であること。商・余りの零多項式は空列で返す。 |
| `f << k`、`f >> k` | `k >= 0`。左シフトは先頭に0を `k` 個挿入し、右シフトは先頭の係数を最大 `k` 個除く。 |
| `f.pre(k)`、`f.pred(k)` | 正確に `k` 係数へ切り詰めるか0で埋める。`pre` はコピー、`pred` はその場で更新して `FPS&` を返す。 |
| `f.rev()`、`f.reversed()` | 格納した係数の順序を逆にする。`rev` はコピー、`reversed` はその場で更新して `FPS&` を返す。 |
| `f.shrink()` | 末尾の0を除き `FPS&` を返す。 |
| `f.dot(g)` | `g.size()` 個の位置を係数ごとに乗算する。左辺が長いときは左辺の残りを保持し、右辺が長いときは左辺を0で補う。戻り値は `FPS`。 |
| `f.eval(x)` | Horner法で $f(x)$ を計算し、係数型で返す。 |

二項演算には対応する複合代入もある。シフト、サイズ変更、係数の加減算・スカラー演算、単点評価は入出力の係数数に対して線形時間、積と2種類の除算は同程度の入力長なら $O(M(n))$ 時間。

### 微積分・逆数・対数・指数・冪・平方根

```cpp
FPS& diffed();
FPS& integraled();
FPS diff() const;
FPS integral() const;
FPS inv(int N = -1) const;
FPS log(int N = -1) const;
FPS exp(int N = -1) const;
FPS pow(long long k, int N = -1) const;
FPS sqrt(int N = -1) const;
FPS sqrt_general(int N = -1) const;
```

| API | 結果と条件 |
| --- | --- |
| `diff()` / `diffed()` | 形式微分。空列は空のまま、その他は1係数減る。`diffed` はその場で更新する。 |
| `integral()` / `integraled()` | 積分定数0で形式積分。長さ $n+1$。$1,\ldots,n$ が可逆であること。`integraled` はその場で更新する。 |
| `inv(N)` | $f^{-1}\bmod x^N$。$N>0$ では定数項が可逆であること。 |
| `log(N)` | $\log f\bmod x^N$。定数項1で、$1,\ldots,N-1$ が可逆であること。 |
| `exp(N)` | $\exp f\bmod x^N$。定数項0で、$1,\ldots,N-1$ が可逆であること。空入力も零級数として使える。 |
| `pow(k,N)` | $f^k\bmod x^N$。`k >= 0`。0乗は1。$c$ を先頭の非零次数とし、$k>0$ かつ $ck<N$ の非零入力では先頭係数と $1,\ldots,N-ck-1$ が可逆であること。 |
| `sqrt(N)` | 定数項1の平方根。正の精度では入力の定数項が1で、$N>1$ なら2が可逆であること。 |
| `sqrt_general(N)` | 奇素数法で一般の平方根を求める。$x^N$ 未満に非零項がなければ長さ `N` の零級数を返す。先頭の非零次数が奇数、またはその係数が平方非剰余なら空を返す。先頭の非零係数に対し `modsqrt` が選ぶ根を使う。 |

微積分は $O(n)$、級数演算は $O(M(N))$ 時間。`pow` では先頭係数のスカラー冪に $O(\log(k+2))$、一般平方根では法に依存する `modsqrt` の計算が加わる。一般平方根を使う翻訳単位では `mod_sqrt.hpp` も読み込む。FPSより先に読み込むと、係数型が他の名前空間にある場合にも `modsqrt` を参照できる。[mod_sqrt](mod_sqrt.md)

### 疎な級数

疎入力は次数と係数の組 `{i,a}` で $a x^i$ を表す。次数が非負で一意になるようまとめ、昇順に並べる。引数 `deg` は出力の係数数。

```cpp
FPS times_sparse(const std::vector<std::pair<int,T>>& x, int deg = -1) const;
FPS div_sparse(const std::vector<std::pair<int,T>>& x, int deg = -1);
static FPS inv_sparse(const std::vector<std::pair<int,T>>& x, int deg);
static FPS log_sparse(const std::vector<std::pair<int,T>>& x, int deg);
static FPS exp_sparse(const std::vector<std::pair<int,T>>& x, int deg);
static FPS pow_sparse(std::vector<std::pair<int,T>> x, long long m, int deg);
static FPS sqrt_sparse(const std::vector<std::pair<int,T>>& x, int deg);
static FPS sqrt_general_sparse(std::vector<std::pair<int,T>> x, int deg);
```

- `times_sparse` と `div_sparse` は左辺との積・級数除算。精度省略時は左辺の長さ。`div_sparse` には正の精度を渡す。
- `inv_sparse` と `div_sparse` の先頭に `{0, 可逆な定数項}`、`log_sparse` と `sqrt_sparse` の先頭に `{0, 1}` を置く。`exp_sparse` は定数項を省く。
- `pow_sparse` と `sqrt_general_sparse` は先頭に最小次数の非零項を置く。冪には非負の指数を渡し、`deg + m` が `long long` に収まること。一般平方根は奇素数法で、次数 `deg` 未満の項を渡す。解なしは空で返る。
- 対数・指数・冪・平方根の係数漸化式では、計算途中の係数添字で割るため、必要な正整数の逆元も用意する。`sqrt_sparse` では2の逆元も使う。`_pow_sparse(x,T(m),deg)` は定数項1のべき級数用の補助手続き。

疎入力の項数を $s$ とすると $O((s+1)\,\mathrm{deg})$ 時間。冪と一般平方根では先頭係数に対するスカラー冪・平方根の計算も加わる。

### 多点評価・補間・基底変換

```cpp
std::vector<T> mult_eval(const std::vector<T>& p) const;
static FPS interpolation(const std::vector<T>& x, const std::vector<T>& y);
std::vector<T> geo_eval(T a, T r, int m);
static FPS geo_interpolation(const std::vector<T>& y, T a, T r);
std::vector<T> to_newton(const std::vector<T>& p) const;
std::vector<T> NewtonBasis(std::vector<T> p) const;
FPS Taylor_shift(T a) const;
static std::vector<T> shift_sampling(std::vector<T> f, T c, int m = -1);
FPS Mobius_Transform(T a, T b, T c, T d) const;
```

| API | 結果と条件 |
| --- | --- |
| `mult_eval(p)` | 各点 `p[i]` で評価し、入力の点順で返す。点は重複可。 |
| `interpolation(x,y)` | `x.size() == y.size()`。$f(x_i)=y_i$ を満たす次数 $<x.size()$ の多項式を、点の数と同じ長さで返す。すべての点の差が可逆であること。 |
| `geo_eval(a,r,m)` | $f(ar^i)$ を $0\le i<m$ について返す。`m >= 0`。`r == 0` と `r == 1` は専用処理、それ以外では `r` が可逆であること。 |
| `geo_interpolation(y,a,r)` | $ar^i$ における値 `y[i]` から `y.size()` 係数を返す。2点以上では `a` と `r` が可逆で、すべての点の差が可逆であること。 |
| `to_newton(p)`、`NewtonBasis(p)` | `p.size() == f.size()`。基底 $1,(x-p_0),(x-p_0)(x-p_1),\ldots$ での係数列を返す。点は重複可。 |
| `Taylor_shift(a)` | $f(x+a)$ を元の係数数で返す。$1,\ldots,n-1$ が可逆であること。 |
| `shift_sampling(values,c,m)` | `values = {f(0), …, f(n-1)}` から $f(c),\ldots,f(c+m-1)$ を返す。$\deg f<n$ を仮定する。非空入力で `m` を省略すると **`n-1`** 点。回収したい点数が決まっているなら明示する。必要な逆階乗の添字は最大 $\max(n-1,m-1)$。 |
| `Mobius_Transform(a,b,c,d)` | $D=n-1$ とし、$\sum_{i=0}^D f_i(ax+b)^i(cx+d)^{D-i}$ の $n$ 係数を返す。非零の `c` を使う場合は `c` が可逆であること。必要な階乗・逆階乗は $D$ まで。 |

評価・補間・Newton基底変換は係数数と点数の和を $s$ として $O(M(s)\log(s+2))$ 時間。等比評価・補間、Taylor shift、点列のシフト、Möbius変換は $O(M(s))$ 時間。ここで点列のシフトは $s=n+m$、Taylor shiftとMöbius変換は $s=n$。`shift_sampling(...,1)` は $O(n)$。

### 多項式積・有理式・GCD・根

```cpp
static FPS prod(std::vector<FPS> F);
static std::pair<FPS,FPS> quotient_sum(std::vector<std::pair<FPS,FPS>> QR);
static T Bostan_Mori(FPS F, FPS G, long long k);
static FPS PolyGCD(const FPS& f, const FPS& g);
FPS gcd(const FPS& g) const;
static std::pair<bool,FPS> PolyInv(const FPS& f, const FPS& g);
std::pair<bool,FPS> inv_mod(const FPS& g) const;
static T Resultant(const FPS& f, const FPS& g);
T resultant(const FPS& g) const;
std::vector<T> FindRoot() const;
```

- `prod(F)` は多項式列の積。空の列の積は `{1}`。`quotient_sum(QR)` は `{分子, 分母}` の列から和を表す `{分子, 分母}` を返す。分母は末尾の0を除いた非零多項式で、最高次係数が可逆であること。分子と分母は約分せず、空の列では `{{0},{1}}` を返す。両APIともヒープで短い積から併合する。
- `Bostan_Mori(F,G,k)` は $F/G$ を定数項から展開したときの $x^k$ の係数を返す。`k >= 0` で `G[0]` が可逆であること。
- `gcd(g)` は最高次係数を1に正規化したGCD。両方が零なら空を返す。`inv_mod(g)` は非零多項式 `g` を法とする逆元をBézout係数から求める。GCDが定数の場合に `{true, 逆元の代表多項式}`、その他は `{false, {}}`。`resultant(g)` は非零多項式同士の終結式を返す。いずれも対応する静的関数がある。
- `Mat`（`a,b,c,d` を持つ2×2行列、単位行列 `I()`、積、列ベクトルへの適用 `app(p)`）、`Naive_GCD(Mat&, pair<FPS,FPS>&, T&, int)`、`Half_GCD(pair<FPS,FPS>, T&, int pre = 0)`、`GCD_Mat(f,g)` は多項式Euclid法の補助手続き。係数だけが必要なら `gcd`、`inv_mod`、`resultant` を使う。
- `FindRoot()` は法がコンパイル時に決まる奇素数のmodintで、非零多項式の相異なる根を順不同で返す。定数入力では空。まず $\gcd(f,x^p-x)$ で法 $p$ の根だけを抽出し、乱択分割で求める。

Bostan–Moriは $s=\max(F.size(),G.size())$ として $O(M(s)\log(k+2))$ 時間。多項式GCD・逆元・終結式はHalf GCDで $O(M(s)\log(s+2))$ 時間。根の抽出にはこれに加え、法 $p$ に対する冪剰余と、乱択分割の各段での冪剰余・GCDがかかる。

### 合成・逆合成・power projection

```cpp
static FPS Composition(FPS F, FPS G, int m = -1);
static FPS Compositional_inverse(FPS F, int m = -1);
static FPS PowerProjection(FPS F, FPS G, int N, int M);
FPS composition(const FPS& g, int d = -1) const;
FPS compositional_inverse(int d = -1) const;

template<class T, bool ntt_friendly = is_ntt_friendly_v<T>>
struct static_composition {
  using FPS = FormalPowerSeries<T>;
  static_composition(const FPS& g);
  static_composition(const FPS& g, int m);
  FPS composition(FPS f) const;
};

template<class T, int b = 4, bool ntt_friendly = is_ntt_friendly_v<T>>
struct online_composition {
  using FPS = FormalPowerSeries<T>;
  online_composition(FPS g, int n);
  T query(T x);
  T get(int i) const;
};

template<class T, class Factorials = modint_factorials<T>>
std::vector<T> CompositionalPowerProjection(FormalPowerSeries<T> F, int n = -1, int m = -1, FormalPowerSeries<T> C1 = {}, FormalPowerSeries<T> C2 = {}, FormalPowerSeries<T> D = {});
```

- `f.composition(g,m)` は有限多項式 $f(g(x))\bmod x^m$ の先頭 `m` 係数。`g[0] != 0` なら `f` の高次係数も結果に寄与する。省略時は両入力長の最大値。
- `f.compositional_inverse(m)` は $f(g(x))=x\bmod x^m$ となる `g` を返す。精度2以上では `f[0] == 0` で一次係数が可逆であること。省略時は `f.size()`。Lagrange反転に必要な逆元を使える素数法ではpower projectionを使い、他の法・精度ではNewton法を使う。
- `PowerProjection(f,g,n,m)` は `m` 係数の結果を返し、第 $i$ 要素は $[x^{n-1}]g(x)f(x)^i$。次数指定と出力数は独立し、`n == 0` では全要素0。
- `static_composition<T>(g,m)` は内側の `g` と精度を固定し、複数の外側の係数列に対して使える。精度省略時は `g.size()`。`ntt_friendly=false` は合成の前計算に係数側の実装を使う指定。
- `online_composition<T>(g,n)` は定数項0の内側を固定する。外側の係数を低次順に最大 `n` 個 `query(f[i])` へ渡すと、そのたびに合成結果の $i$ 次係数が確定する。`get(i)` は $0\le i<n$ で、未入力の係数を0とみなした値を返す。入力済み個数までの添字は $O(1)$、それより先はpower projectionで計算する。ブロック幅は $2^b$（`0 <= b < 30`）。NTT版がブロック計算で使う指数・対数のため、精度 `n` に必要な整数の逆元を用意する。
- `CompositionalPowerProjection(F,n,m,C1,C2,D)` は $[x^{n-1}]D(x)\,(C_2\circ F^{\circ i}\circ C_1)(x)$ を $i=0,\ldots,m-1$ について返す。$F^{\circ 0}=x$。`F` と `C1` の定数項は0。空の `C1`、`C2` は恒等写像、空の `D` は1。`n` の省略値は `F.size()`、`m` の省略値は `n`。線形化や放物型の正規形を使える場合は結果をまとめて計算する。

合成系には非NTT法や動的modintでも使える係数側の実装がある。精度と入力長が同程度の $N$ なら、合成・逆合成・power projectionは $O(M(N)\log(N+2))$ 時間。合成の事前計算は $O(N\log(N+2))$ 領域。定数項が非零の場合は、合成の外側の格納長またはpower projectionの出力長に応じた畳み込みも行う。反復合成にフォールバックした `CompositionalPowerProjection` は $O(m M(n)\log(n+2))$ 時間。[合成の内部実装](internal/internal_fps_composition.md)

### 整数点での累積和・累積積

```cpp
T prefix_product(long long k) const;
FPS prefix_sum() const;
T sum_of_exponential_times_polynomial(T r, long long n) const;
T sum_of_exponential_times_polynomial_limit(T r) const;
static T sum_of_exponential_times_polynomial(std::vector<T> values, T r, long long n);
static T sum_of_exponential_times_polynomial_limit(const std::vector<T>& values, T r);
```

- `prefix_product(k)` は `k >= 0` に対し $\prod_{0\le i<k} f(i)$ を返す。空積は1。小さい `k` は単点評価、ほかは多点評価またはブロックごとの点列シフトを用いる。大きい `k` を渡す場合は非空の係数列を用意し、点数やブロック幅・境界の計算が `int` の範囲に収まること。点列シフトを使う経路には、2や必要な階乗の逆元も要る。
- `prefix_sum()` は $g(0)=0$、$g(x+1)-g(x)=f(x)$ を満たす多項式を返す。非負整数 $k$ に対し $g(k)=\sum_{0\le i<k}f(i)$。非空の入力長 $n$ に対して長さ $n+1$、空入力では空。$1,\ldots,n$ が可逆であること。Bernoulli数は $B_1=-1/2$ の規約。
- `sum_of_exponential_times_polynomial(r,n)` は `n >= 0` に対し $\sum_{i=0}^{n-1} f(i)r^i$ を返す。`_limit(r)` が求めるのは母関数 $\sum_{i\ge0} f(i)t^i$ を有理式にして $t=r$ で評価した値で、`r != 1` が前提。空入力と空和は0。`r == 0` では非空和は $f(0)$。
- 係数を渡す形式で級数を使う場合、$r\ne 1$ なら $1-r$ と $1,\ldots,\deg f$、$r=1$ の有限和では $1,\ldots,\deg f+1$ が可逆であること。短い有限和は各点で直接評価する。
- 静的オーバーロードは $\deg f\le d$ として `values = {f(0),...,f(d)}` を受け取る。補間・外挿には $1,\ldots,d+1$ の逆元が必要。有限和の外挿で `r` が0、1以外なら `r` と $1-r$ が可逆であること。入力済みの項だけを使う有限和は直接計算する。

`prefix_sum` は $O(M(n))$ 時間。`prefix_product` は直接評価なら $O(nk)$、多点評価なら $O(M(n+k)\log(n+k+2))$。ブロック幅 $B$ を使う場合は $D=n-1$ として、初期評価 $O(n^2)$ に加え、シフトに $O(M(DB+\lceil k/B\rceil+1)+\log(B+1))$、末尾の多点評価に $O(M(n+B)\log(n+B+2))$ 時間。重み付き和は係数から計算する場合 $O(M(n_f)+\log(n+2))$、値列から計算する場合 $O(n_f+\log(n+2))$ 時間。$n_f$ は入力の係数数または値の数。無限和には $\log(n+2)$ の項がない。

### 線形漸化式の復元と外挿

```cpp
static FPS find_linear_recurrence(FPS samples);
static T kth_term_of_linear_recurrence(const FPS& initial, const FPS& recurrence, long long k);
static FPS linear_recurrence_terms(const FPS& initial, const FPS& recurrence, long long k, int count);
static T kth_term_of_linear_recurrence(const FPS& samples, long long k);
static FPS linear_recurrence_terms(const FPS& samples, long long k, int count);
```

`find_linear_recurrence(samples)` は有限の初項列に合う最短の漸化式 `c` を返す。次数 $d=c.size()$ に対し、$i\ge d$ で $a_i=\sum_{j=0}^{d-1}c_j a_{i-1-j}$。末尾の0も次数の一部なので `shrink()` を呼ばない。空・全0の入力では次数0。

`initial` は先頭 $d$ 項、`k` は0始まりで非負。単項取得は $a_k$、連続項取得は $a_k,\ldots,a_{k+\mathrm{count}-1}$ を返し、`count >= 0`。次数0の漸化式は全項0。漸化式を省略すると、要求範囲が入力済みならその値を返し、外挿が必要なら復元した最短漸化式に従う。

復元は初項数 $n$ に対し $O(M(n)\log(n+2))$ 時間。次数 $d$ が既知なら、単項取得はBostan–Moriで $O(M(d+1)\log(k+2))$、連続 $m$ 項は逆数の係数窓を使い $O(M(d+1)\log(k+2)+M(d+m+1))$ 時間。[Bostan–Mori](https://arxiv.org/abs/2008.08822) / [逆数の連続係数の計算](https://noshi91.hatenablog.com/entry/2023/06/04/233447)

## 計算量

各APIの表記は必要な係数演算が定数時間で、畳み込みに通常のNTTまたは固定個数のNTTとCRTを使う場合を基準とする。このとき $M(N)=O(N\log(N+2))$。直接積へ切り替わる入力では、積の項数に応じて時間が増える。階乗表を使う操作で、未計算の整数まで表を伸ばす場合は前計算の時間・領域が加わる。

密な級数の演算、等比評価、累積和・重み付き和の作業領域は入出力長に対して線形。積木を使う多点評価・補間・Newton基底変換は、係数数と点数の和を $s$ として $O(s\log(s+2))$ 領域。単独のpower projectionは精度と出力数に対して線形、合成の前計算は各段の分母を保持する。連続漸化式の $m$ 項取得は $O(d\log(k+2)+m)$ 作業領域。

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
- 一般平方根を呼ぶ翻訳単位で追加するヘッダー。[mod_sqrt.hpp](mod_sqrt.md)

## Verify・利用例

```cpp
#include <cassert>
#include "library/math/modint.hpp"
#include "library/math/formal_power_series.hpp"

using Mint = soryuusi::modint998244353;
using FPS = soryuusi::FormalPowerSeries<Mint>;

int main() {
  FPS f{1, 2, 3};
  auto series = f / FPS{1, -1};  // f/(1-x) mod x^3
  assert(series == (FPS{1, 3, 6}));
  auto [q, r] = f.div(FPS{-1, 1});  // f = (x-1)q + r
  assert(q == (FPS{5, 3}) && r == FPS{6});

  FPS g{0, 1, 1};
  auto inverse = g.compositional_inverse(5);
  assert(g.composition(inverse, 5) == (FPS{0, 1, 0, 0, 0}));

  FPS fib{0, 1, 1, 2, 3, 5};
  auto recurrence = FPS::find_linear_recurrence(fib);
  assert(recurrence == (FPS{1, 1}));
  assert(FPS::linear_recurrence_terms(fib, 6, 3) == (FPS{8, 13, 21}));
}
```

### ユニットテストとLibrary Checker

- [`unit/formal_power_series`](../../../verify/unit/formal_power_series.cpp)
- [`unit/combinatorics`](../../../verify/unit/combinatorics.cpp)

#### 級数演算・疎な級数

- [`library_checker/exp_of_formal_power_series`](../../../verify/library_checker/polynomial/exp_of_formal_power_series.cpp)
- [`library_checker/exp_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/exp_of_formal_power_series_sparse.cpp)
- [`library_checker/inv_of_formal_power_series`](../../../verify/library_checker/polynomial/inv_of_formal_power_series.cpp)
- [`library_checker/inv_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/inv_of_formal_power_series_sparse.cpp)
- [`library_checker/log_of_formal_power_series`](../../../verify/library_checker/polynomial/log_of_formal_power_series.cpp)
- [`library_checker/log_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/log_of_formal_power_series_sparse.cpp)
- [`library_checker/pow_of_formal_power_series`](../../../verify/library_checker/polynomial/pow_of_formal_power_series.cpp)
- [`library_checker/pow_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/pow_of_formal_power_series_sparse.cpp)
- [`library_checker/sqrt_of_formal_power_series`](../../../verify/library_checker/polynomial/sqrt_of_formal_power_series.cpp)
- [`library_checker/sqrt_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/sqrt_of_formal_power_series_sparse.cpp)

#### 評価・補間・多項式

- [`library_checker/conversion_from_monomial_basis_to_newton_basis`](../../../verify/library_checker/polynomial/conversion_from_monomial_basis_to_newton_basis.cpp)
- [`library_checker/division_of_polynomials`](../../../verify/library_checker/polynomial/division_of_polynomials.cpp)
- [`library_checker/inv_of_polynomials`](../../../verify/library_checker/polynomial/inv_of_polynomials.cpp)
- [`library_checker/multipoint_evaluation`](../../../verify/library_checker/polynomial/multipoint_evaluation.cpp)
- [`library_checker/multipoint_evaluation_on_geometric_sequence`](../../../verify/library_checker/polynomial/multipoint_evaluation_on_geometric_sequence.cpp)
- [`library_checker/polynomial_interpolation`](../../../verify/library_checker/polynomial/polynomial_interpolation.cpp)
- [`library_checker/polynomial_interpolation_on_geometric_sequence`](../../../verify/library_checker/polynomial/polynomial_interpolation_on_geometric_sequence.cpp)
- [`library_checker/polynomial_root_finding`](../../../verify/library_checker/polynomial/polynomial_root_finding.cpp)
- [`library_checker/polynomial_taylor_shift`](../../../verify/library_checker/polynomial/polynomial_taylor_shift.cpp)
- [`library_checker/product_of_polynomial_sequence`](../../../verify/library_checker/polynomial/product_of_polynomial_sequence.cpp)
- [`library_checker/shift_of_sampling_points_of_polynomial`](../../../verify/library_checker/polynomial/shift_of_sampling_points_of_polynomial.cpp)

#### 合成・逆合成

- [`library_checker/composition_of_formal_power_series_crt`](../../../verify/library_checker/polynomial/composition_of_formal_power_series_crt.cpp)
- [`library_checker/compositional_inverse_of_formal_power_series_crt`](../../../verify/library_checker/polynomial/compositional_inverse_of_formal_power_series_crt.cpp)
- [`library_checker/composition_of_formal_power_series`](../../../verify/library_checker/polynomial/composition_of_formal_power_series.cpp)
- [`library_checker/composition_of_formal_power_series_large`](../../../verify/library_checker/polynomial/composition_of_formal_power_series.cpp)
- [`library_checker/compositional_inverse_of_formal_power_series`](../../../verify/library_checker/polynomial/compositional_inverse_of_formal_power_series.cpp)
- [`library_checker/compositional_inverse_of_formal_power_series_large`](../../../verify/library_checker/polynomial/compositional_inverse_of_formal_power_series.cpp)

#### 累積和・漸化式

- [`library_checker/sum_of_exponential_times_polynomial`](../../../verify/library_checker/other/sum_of_exponential_times_polynomial.cpp)
- [`library_checker/sum_of_exponential_times_polynomial_limit`](../../../verify/library_checker/other/sum_of_exponential_times_polynomial_limit.cpp)
- [`library_checker/kth_term_of_linearly_recurrent_sequence`](../../../verify/library_checker/other/kth_term_of_linearly_recurrent_sequence.cpp)
- [`library_checker/find_linear_recurrence`](../../../verify/library_checker/other/find_linear_recurrence.cpp)
- [`library_checker/consecutive_terms_of_linear_recurrent_sequence`](../../../verify/library_checker/other/consecutive_terms_of_linear_recurrent_sequence.cpp)
- [`library_checker/prefix_sum_of_polynomial`](../../../verify/library_checker/polynomial/prefix_sum_of_polynomial.cpp)

#### 組合せへの応用

- [`library_checker/factorial`](../../../verify/library_checker/enumerative_combinatorics/factorial.cpp)
- [`library_checker/bell_number`](../../../verify/library_checker/enumerative_combinatorics/bell_number.cpp)
- [`library_checker/partition_function`](../../../verify/library_checker/enumerative_combinatorics/partition_function.cpp)
- [`library_checker/sharp_p_subset_sum`](../../../verify/library_checker/enumerative_combinatorics/sharp_p_subset_sum.cpp)
- [`library_checker/bernoulli_number`](../../../verify/library_checker/number_theory/bernoulli_number.cpp)
