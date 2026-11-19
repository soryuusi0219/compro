# convolution

[索引](../README.md) / [実装](../../../library/math/convolution.hpp)

## 機能・前提条件

静的modintまたは整数係数の畳み込み。通常の積、同じ列の二乗、middle productを計算する。
整数係数版は `Mod` で剰余を取り、入力と同じ整数型で返す。係数型は剰余を表せる型を使う。
`convolution_ll` はCRTで符号付き整数係数を復元し、すべての出力が `long long` に収まることを前提とする。

## インターフェース

```cpp
#include "library/math/convolution.hpp"

auto c = soryuusi::convolution(a, b);
auto d = soryuusi::convolution_square(a);
auto mid = soryuusi::middle_product(a, b);
```

| API | 返す係数 |
| --- | --- |
| `convolution(a,b)` | $c=a*b$ の全係数。 |
| `convolution_square(a)` | $a*a$ の全係数。同じスペクトルを二乗する。 |
| `middle_product(a,b)` | $c=a*b$ の係数 $c_{M-1},\ldots,c_{N-1}$。$N$ は `a.size()`、$M$ は `b.size()`。 |
| `convolution_ll(const vector<long long>& a,const vector<long long>& b)` | 符号付き整数の畳み込み。 |

最初の3つは、静的modintの `std::vector<Mint>` を値で受け取る版と、整数の `const std::vector<T>&` を受け取る版を持つ。整数版の先頭テンプレート引数 `Mod` の既定値は998244353。
いずれも空の入力には空配列を返す。middle productは $N<M$ の場合も空配列を返す。

## 計算量

$N,M$ を入力長、$L=N+M-1$ とする。通常は $O(L\log L)$ 時間・$O(L)$ 領域。
短い入力やNTTを使えない法では直接計算する。変換長を少し超える積は、短い巡回畳み込みと上端の係数の補正を使う。入力長の差が大きい場合は短い側のスペクトルをブロック間で再利用する。
最大変換長 $S$ を超えるときは長さ $S/2$ のブロックに分け、ブロックの組が同じ出力ブロックに書き込む成分を周波数側で集約する。両入力のブロック数を $A,B$ とすれば $O((A+B)S\log S+AB S)$ 時間。
middle productの変換長は $N$ 以上の最小の2冪で足りる。使用できる変換長を超える場合は通常畳み込みから必要な区間を取る。

## 依存関係

- [internal_math.hpp](internal_math.md)
- [modint_traits.hpp](modint_traits.md)
- [ntt.hpp](ntt.md)

## Verify・利用例

- [`unit/ntt_convolution`](../../../verify/unit/ntt_convolution.cpp)
- [`library_checker/convolution_mod`](../../../verify/library_checker/convolution/convolution_mod.cpp)
- [`library_checker/convolution_mod_large`](../../../verify/library_checker/convolution/convolution_mod_large.cpp)
