# convolution_online

[索引](../README.md) / [実装](../../../library/math/convolution_online.hpp)

## 機能・前提条件

両数列を1項ずつpushしてその時点の畳み込み係数を返すrelaxed convolution。型と変換可能長に応じてfallbackを使う。

## インターフェース

```cpp
#include "library/math/convolution_online.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template <typename T>
struct online_convolution;
using mint = T;
using vm = vector<T>;
online_convolution() = default;
explicit online_convolution(int lim);
void _cul(int la, int lb, int len);
mint push(mint a0, mint b0);
```

## 計算量

NTT範囲で N push 合計 O(N log² N)、空間 O(N log N) を上界とする。fallbackは畳み込み実装に依存。

## 依存関係

- [convolution_mod.hpp](convolution_mod.md)
- [modint_traits.hpp](modint_traits.md)
- [ntt.hpp](ntt.md)

## Verify・利用例

- [`library_checker/convolution_mod_1000000007`](../../../verify/library_checker/convolution/convolution_mod_1000000007.cpp)
- [`library_checker/convolution_mod_1000000007_online`](../../../verify/library_checker/convolution/convolution_mod_1000000007_online.cpp)
- [`library_checker/convolution_mod_1000000007_semi_online`](../../../verify/library_checker/convolution/convolution_mod_1000000007_semi_online.cpp)
- [`library_checker/convolution_mod_online`](../../../verify/library_checker/convolution/convolution_mod_online.cpp)
