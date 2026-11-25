# convolution_subset

[索引](../README.md) / [実装](../../../library/math/convolution_subset.hpp)

## 機能・前提条件

部分集合畳み込み $c[S]=\sum_{A\subseteq S} a[A]b[S\setminus A]$ を計算する。添字は集合を表すビットマスク。入力長が異なる場合や2の冪でない場合は、長い方の次の2冪まで不足係数を0として扱う。片方が空なら空を返す。

小さな入力は部分集合を列挙する。大きな入力は集合サイズごとのゼータ変換で積を計算し、メビウス反転で同じ要素を両方の集合に割り当てる項を除く。変換表はマスクごとに必要な次数だけを保持する。[Fourier meets Möbius: fast subset convolution](https://arxiv.org/abs/cs/0611101)

## インターフェース

```cpp
#include "library/math/convolution_subset.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
vector<T> convolution_subset(const vector<T>& a, const vector<T>& b);
```

## 計算量

出力長 $L=2^B$ に対して $O((B+1)^2L)$ 時間、$O((B+1)L)$ 領域。大きな入力での変換表1つは $(B/2+1)L$ 係数を持つ。通常の積には2表を使い、同じvectorを両引数に渡す場合は1表を使う。

## 依存関係

- [modint_traits.hpp](internal/modint_traits.md)
- [internal_bit.hpp](internal/internal_bit.md)

## Verify・利用例

- [`library_checker/subset_convolution`](../../../verify/library_checker/set_power_series/subset_convolution.cpp)
- [`unit/set_power_series`](../../../verify/unit/set_power_series.cpp)
