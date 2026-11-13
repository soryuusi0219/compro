# monotone_minima

[索引](../README.md) / [実装](../../../library/utility/monotone_minima.hpp)

## 機能・前提条件

行最小位置が単調な行列をcallbackで評価して各行の (列,値) を返す。Compareで最小・最大を選ぶ。

## インターフェース

```cpp
#include "library/utility/monotone_minima.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class F, class Compare = less<T>>
vector<pair<int,T>> monotoneminima(int h, int w, const F& f, const Compare &comp = Compare());
```

## 計算量

H×Wに O(H+W log H) 回程度の評価（O((H+W) log H)を上界）、O(H+log H) 補助空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/min_plus_convolution_convex_arbitrary_monotone_minima_utility`](../../../verify/library_checker/convolution/min_plus_convolution_convex_arbitrary_monotone_minima_utility.cpp)
