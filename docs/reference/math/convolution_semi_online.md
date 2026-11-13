# convolution_semi_online

[索引](../README.md) / [実装](../../../library/math/convolution_semi_online.hpp)

## 機能・前提条件

片方の数列を固定し、もう片方をpushして畳み込み係数を順次得る。initで固定列を指定する。

## インターフェース

```cpp
#include "library/math/convolution_semi_online.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template <typename T>
struct semi_online_convolution;
using mint = T;
using vm = vector<T>;
semi_online_convolution() = default;
explicit semi_online_convolution(vm b0);
void init(vm b0);
void _cul(int la, int lb, int len);
mint push(mint a0);
```

## 計算量

NTT範囲で N push 合計 O(N log² N)、空間 O(N log N) を上界とする。fallbackは型に依存。

## 依存関係

- [convolution_mod.hpp](convolution_mod.md)
- [modint_traits.hpp](modint_traits.md)
- [ntt.hpp](ntt.md)

## Verify・利用例

- [`library_checker/convolution_mod_semi_online`](../../../verify/library_checker/convolution/convolution_mod_semi_online.cpp)
