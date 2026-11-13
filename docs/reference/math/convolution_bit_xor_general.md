# convolution_bit_xor_general

[索引](../README.md) / [実装](../../../library/math/convolution_bit_xor_general.hpp)

## 機能・前提条件

2が可逆でない係数環にも対応するXOR畳み込み。次数別の配列に持ち上げて計算する。

## インターフェース

```cpp
#include "library/math/convolution_bit_xor_general.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
vector<T> convolution_bitxor_general(vector<T> a, vector<T> b);
```

## 計算量

長さ L=2^B に O(LB²) 時間、O(LB) 空間を上界とする。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/bitwise_xor_convolution_general`](../../../verify/library_checker/convolution/bitwise_xor_convolution_general.cpp)
