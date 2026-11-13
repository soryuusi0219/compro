# convolution_bit_xor

[索引](../README.md) / [実装](../../../library/math/convolution_bit_xor.hpp)

## 機能・前提条件

bit XOR畳み込み。Hadamard変換の逆で2冪による除算を行うため、その除算が可能な係数型を使う。

## インターフェース

```cpp
#include "library/math/convolution_bit_xor.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
vector<T> convolution_bitxor(vector<T> a,vector<T> b);
```

## 計算量

長さ L=2^B に O(LB) 時間、O(L) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/bitwise_xor_convolution`](../../../verify/library_checker/convolution/bitwise_xor_convolution.cpp)
