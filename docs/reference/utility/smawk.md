# smawk

[索引](../README.md) / [実装](../../../library/utility/smawk.hpp)

## 機能・前提条件

全単調行列の各行最小をcallback評価で求めるSMAWK。単なる最小位置の単調性より強い条件が必要。

## インターフェース

```cpp
#include "library/utility/smawk.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class Compare = less<T>>
vector<pair<int,T>> SMAWK(int h, int w, const auto f, const Compare &comp = Compare());
```

## 計算量

H×Wに O(H+W) 回の評価、O(H+W) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/min_plus_convolution_convex_arbitrary_smawk_utility`](../../../verify/library_checker/convolution/min_plus_convolution_convex_arbitrary_smawk_utility.cpp)
