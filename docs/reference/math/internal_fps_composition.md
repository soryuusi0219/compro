# internal_fps_composition

[索引](../README.md) / [実装](../../../library/math/internal_fps_composition.hpp)

## 機能・前提条件

形式的冪級数の合成とpower projectionで使う内部実装。精度は非負。NTT版の合成では内側の定数項を0とし、必要な変換長を法が持つ場合に使う。

二変数の分母を $Q(x,y)=1-y g(x)$ とする。係数側の実装では $Q=E(x^2,y)+xO(x^2,y)$ と分け、次の分母を $E^2-xO^2$ として計算する。分子の奇数次に対する更新を転置すると合成になる。平坦化した各行に十分な幅を取り、$x$ の次数が $y$ 側に繰り上がらないようにする。
NTT版では各段の分母を周波数側に保持し、$y$ 方向の奇数周波数の拡張とその転置を用いる。

## インターフェース

```cpp
// 公開側から利用する。
#include "library/math/formal_power_series.hpp"
auto coefficients = f.composition(g, n);
auto projections = decltype(f)::PowerProjection(g, f, n, m);
```

- `FPSCompositionPlan<FPS>` は内側の級数と精度を受け取り、`composition(FPS)` を繰り返し実行できる。
- `FPSCompositionCRT<FPS>::project(f,g,n,m)` は、積の係数を $i=0,\ldots,m-1$ に対して計算する。$x$ の精度は $n$。
- `fps_product` と `fps_middle_product` は、係数型と復元範囲に応じてNTT、CRT、直接計算を選ぶ。

## 計算量

内側の定数項が0で、対象の係数数が $N$ の場合、畳み込み時間を $M(N)$ として $O(M(N)\log N)$ 時間。係数側のpower projectionは現段だけを保持するため $O(N)$ 領域。合成の事前計算は全段の分母を保持するため $O(N\log N)$ 領域。内側の定数項が0以外の場合には、外側の係数数または要求する出力数に応じた追加の一次元演算がある。

## 依存関係

- [convolution.hpp](convolution.md)
- [convolution_mod.hpp](convolution_mod.md)
- [modint_traits.hpp](modint_traits.md)
- [ntt.hpp](ntt.md)

## Verify・利用例

- [`unit/formal_power_series`](../../../verify/unit/formal_power_series.cpp)
- [`library_checker/composition_of_formal_power_series_crt`](../../../verify/library_checker/polynomial/composition_of_formal_power_series_crt.cpp)
- [Kinoshita and Li, Power Series Composition in Near-Linear Time](https://arxiv.org/abs/2404.05177)
