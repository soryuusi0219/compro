# convolution_gcd_lcm

[索引](../README.md) / [実装](../../../library/math/convolution_gcd_lcm.hpp)

## 機能・前提条件

添字のgcd/lcmに従う畳み込み。添字1以上を主対象とする約数・倍数変換を使う。

## インターフェース

```cpp
#include "library/math/convolution_gcd_lcm.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
vector<T> convolution_gcd(vector<T> a,vector<T> b);
template<class T>
vector<T> convolution_lcm(vector<T> a,vector<T> b);
```

## 計算量

長さ N に O(N log N) 時間を上界とし、O(N) 空間。

## 依存関係

- [prime_table.hpp](prime_table.md)

## Verify・利用例

- [`library_checker/gcd_convolution`](../../../verify/library_checker/convolution/gcd_convolution.cpp)
- [`library_checker/lcm_convolution`](../../../verify/library_checker/convolution/lcm_convolution.cpp)
