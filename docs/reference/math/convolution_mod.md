# convolution_mod

[索引](../README.md) / [実装](../../../library/math/convolution_mod.hpp)

## 機能・前提条件

複数NTT素数とCRTによる任意mod畳み込み。整数版とmodint版がある。中間積の復元可能範囲を守る。

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
- [modint_traits.hpp](modint_traits.md)

## Verify・利用例

直接includeするverifyはありません。依存先からの間接利用は `make report` で確認できます。
