# ntt

[索引](../README.md) / [実装](../../../library/math/ntt.hpp)

## 機能・前提条件

静的modintのNTTエンジン。ntt/inttと正規化なし逆変換、iterator/range API。長さはmodが許す2冪。

## インターフェース

```cpp
#include "library/math/ntt.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template <class Mint, internal::is_static_modint_like_t<Mint>* = nullptr>
struct NTTEngine;
static constexpr Mint root(int level);
static constexpr Mint inv_root(int level);
static constexpr Mint rate2(int index);
static constexpr Mint inv_rate2(int index);
static constexpr Mint rate3(int index);
static constexpr Mint inv_rate3(int index);
template <std::random_access_iterator It>
static bool can_use_avx512();
template <std::random_access_iterator It>
static void ntt(It first, It last);
template <std::ranges::random_access_range Range>
static void ntt(Range& range);
template <std::random_access_iterator It>
static void intt_unnormalized(It first, It last);
template <std::ranges::random_access_range Range>
static void intt_unnormalized(Range& range);
template <std::random_access_iterator It>
static void intt(It first, It last);
template <std::ranges::random_access_range Range>
static void intt(Range& range);
```

## 計算量

長さNに O(N log N) 時間、変換配列 O(N)、補助根表 O(log mod)。

## 依存関係

- [internal_math.hpp](internal_math.md)
- [modint_traits.hpp](modint_traits.md)

## Verify・利用例

直接includeするverifyはありません。依存先からの間接利用は `make report` で確認できます。
