# convolution

[索引](../README.md) / [実装](../../../library/math/convolution.hpp)

## 機能・前提条件

静的modintまたは整数係数の通常畳み込み。modのNTT可能長を超える場合はブロック分割を使う。

## インターフェース

```cpp
#include "library/math/convolution.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template <unsigned int Mod>
class convolution_mint;
static constexpr unsigned int mod();
static constexpr convolution_mint raw(unsigned int value);
constexpr convolution_mint() = default;
template <class T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
constexpr convolution_mint(T value);
constexpr unsigned int val() const;
constexpr convolution_mint& operator+=(const convolution_mint& rhs);
constexpr convolution_mint& operator-=(const convolution_mint& rhs);
constexpr convolution_mint& operator*=(const convolution_mint& rhs);
constexpr convolution_mint pow(long long exponent) const;
constexpr convolution_mint inv() const;
template <class Mint>
std::vector<Mint> convolution_ntt(std::vector<Mint> a, std::vector<Mint> b);
template <class Mint>
std::vector<Mint> convolution_large(std::vector<Mint> a, std::vector<Mint> b);
template <class Mint, internal::is_static_modint_like_t<Mint>* = nullptr>
std::vector<Mint> convolution(std::vector<Mint> a, std::vector<Mint> b);
template <unsigned int Mod = 998244353, class T,
std::vector<T> convolution(const std::vector<T>& a, const std::vector<T>& b);
```

## 計算量

通常のNTT範囲で O(L log L)、空間 O(L)。小入力は O(NM)。最大変換長 S を超える場合はブロック対ごとの O(S log S) が必要。

## 依存関係

- [ntt.hpp](ntt.md)
- [modint_traits.hpp](modint_traits.md)

## Verify・利用例

- [`library_checker/convolution_mod`](../../../verify/library_checker/convolution/convolution_mod.cpp)
- [`library_checker/convolution_mod_large`](../../../verify/library_checker/convolution/convolution_mod_large.cpp)
