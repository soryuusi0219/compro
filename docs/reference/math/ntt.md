# ntt

[索引](../README.md) / [実装](../../../library/math/ntt.hpp)

## 機能・前提条件

静的modint上のNTT。法は符号付き32 bitに収まる素数で、変換長 $N$ は空でない2冪かつ `N <= 1 << NTTEngine<Mint>::rank2` とする。
正変換では原始 $N$ 乗根を $\omega$ として $f(\omega^j)$ を求め、添字 $j$ のbit反転位置に格納する。逆変換は同じ順序を受け取る。
連続格納のiteratorではCPUに応じてAVX2またはAVX-512を使う。その他のランダムアクセスiteratorはスカラ実装を使う。

## インターフェース

```cpp
#include "library/math/ntt.hpp"

using NTT = soryuusi::NTTEngine<Mint>;
NTT::ntt(a);
NTT::ntt_double(a);  // 係数列をゼロ埋めした2倍長のスペクトル
NTT::ntt_shift(a);   // x f(x) のスペクトル
NTT::intt(a);
```

変換・根表のAPIは次のとおり。変換はiteratorの組または可変rangeを受け取る。

```cpp
template <class Mint, internal::is_static_modint_like_t<Mint>* = nullptr>
struct NTTEngine;

static constexpr Mint root(int level);
static constexpr Mint inv_root(int level);
static constexpr Mint rate2(int index);
static constexpr Mint inv_rate2(int index);
static constexpr Mint rate3(int index);
static constexpr Mint inv_rate3(int index);
static Mint inverse_size(int n);
template <std::random_access_iterator It>
static bool can_use_avx512();
template <std::random_access_iterator It>
static void ntt(It first, It last);
template <std::random_access_iterator It>
static void intt(It first, It last);
template <std::random_access_iterator It>
static void intt_unnormalized(It first, It last);
template <std::random_access_iterator It>
static void normalize(It first, It last, int n);
```

`intt_unnormalized` の出力は係数を $N$ 倍したもの。`inverse_size(N)` は $1/N$ を返す。
`normalize(first,last,N)` では未正規化の逆変換から必要な範囲を指定できる。

以下の操作はNTT済みの配列を受け取る。可変引数の操作は入力を更新する。
$N$ を入力スペクトルの長さとし、2倍長を扱う操作は $2N$ が変換可能であること、半分の長さを求める操作は $N\geq 2$ を前提とする。

| API | 結果 | 計算量 |
| --- | --- | --- |
| `ntt_odd_extend(span<Mint>)` | ゼロ埋めした $2N$ 点のうち奇数周波数だけを、元の配列に書き込む。 | $O(N\log(N+1))$ |
| `ntt_odd_extend_transposed(span<Mint>)` | `ntt_odd_extend` の内積に関する転置。 | $O(N\log(N+1))$ |
| `ntt_double(vector<Mint>&)` | ゼロ埋めした $2N$ 点への変換。既存の $N$ 点を前半に再利用する。 | $O(N\log(N+1))$ |
| `ntt_negate_argument(span<Mint>)` | $f(-x)$ のスペクトル。 | $O(N)$ |
| `ntt_square_argument(vector<Mint>&)` | $f(x^2)$ の $2N$ 点スペクトル。 | $O(N)$ |
| `ntt_even_product(vector<Mint>&)` | $g(x)g(-x)=f(x^2)$ を満たす $f$ の $N/2$ 点スペクトル。$f$ は $x^{N/2}-1$ で巡回する。 | $O(N)$ |
| `ntt_shift(span<Mint>)` | $xf(x)$ のスペクトル。係数は $x^N-1$ で巡回する。 | $O(N)$ |
| `intt_parity(span<const Mint>, bool odd)` | 偶数添字（`false`）または奇数添字（`true`）の元の係数列。 | $O(N\log N)$ |
| `ntt_half(span<const Mint>, bool high)` | 低次半分（`false`）または高次半分（`true`）の係数列の $N/2$ 点スペクトル。 | $O(N\log N)$ |
| `ntt_split(span<const Mint>)` | 低次・高次のスペクトルの組。両方の計算で変換を共有する。 | $O(N\log N)$ |
| `ntt_inverse_argument(span<Mint>)` | $f(x^{-1})$ のスペクトル。 | $O(N)$ |
| `ntt_reverse(span<Mint>)` | 係数列を反転した多項式のスペクトル。 | $O(N)$ |

## 計算量

正・逆変換は長さ $N$ に対し $O(N\log(N+1))$ 時間。入力配列と根表 $O(\log(\mathrm{mod}))$ を使う。上の表で配列を拡張する操作と、別の配列を返す操作の領域は $O(N)$。

## 依存関係

- [internal_math.hpp](internal_math.md)
- [modint_traits.hpp](modint_traits.md)

## Verify・利用例

- [`unit/ntt_convolution`](../../../verify/unit/ntt_convolution.cpp)
- [`library_checker/convolution_mod`](../../../verify/library_checker/convolution/convolution_mod.cpp)
- [`library_checker/kth_term_of_linearly_recurrent_sequence`](../../../verify/library_checker/other/kth_term_of_linearly_recurrent_sequence.cpp)
