# convolution_mod

[索引](../README.md) / [実装](../../../library/math/convolution_mod.hpp)

## 機能・前提条件

複数NTT素数とCRTによる任意mod畳み込み。整数版の法は正で、入力の負の係数も法で正規化する。CRTを使う場合は、非負剰余に直した係数の整数畳み込みの各項が3つのNTT素数の積未満であること。短い入力は指定の法で直接計算する。

## インターフェース

```cpp
#include "library/math/convolution_mod.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<long long MOD = 1000000007>
vector<long long> convolution_mod(const vector<long long>& a, const vector<long long>& b);
template <class Mint, internal::is_modint_like_t<Mint>* = nullptr>
vector<Mint> convolution_mod(const vector<Mint>& a, const vector<Mint>& b);
```

## 計算量

L=N+M に対し通常 O(L log L) 時間、O(L) 空間。

## 依存関係

- [convolution.hpp](convolution.md)
- [internal_math.hpp](internal_math.md)
- [modint_traits.hpp](modint_traits.md)

## Verify・利用例

- [`unit/ntt_convolution`](../../../verify/unit/ntt_convolution.cpp)
- [`library_checker/convolution_mod_1000000007`](../../../verify/library_checker/convolution/convolution_mod_1000000007.cpp)
