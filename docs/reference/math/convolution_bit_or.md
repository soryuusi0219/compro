# convolution_bit_or

[索引](../README.md) / [実装](../../../library/math/convolution_bit_or.hpp)

## 機能・前提条件

bit OR畳み込み。部分集合ゼータ変換と逆変換を使う。

## インターフェース

```cpp
#include "library/math/convolution_bit_or.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
vector<T> convolution_bitor(vector<T> a,vector<T> b);
```

## 計算量

長さ L=2^B に O(LB) 時間、O(L) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/bitwise_or_convolution`](../../../verify/library_checker/convolution/bitwise_or_convolution.cpp)
