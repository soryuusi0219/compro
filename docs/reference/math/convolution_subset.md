# convolution_subset

[索引](../README.md) / [実装](../../../library/math/convolution_subset.hpp)

## 機能・前提条件

互いに素な部分集合分割について積を足すsubset convolution。両入力は対応する2冪長とする。

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

長さ 2^B に O(B² 2^B) 時間、O(B 2^B) 空間。

## 依存関係

- [modint_traits.hpp](modint_traits.md)

## Verify・利用例

- [`library_checker/subset_convolution`](../../../verify/library_checker/set_power_series/subset_convolution.cpp)
