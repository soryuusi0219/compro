# internal_bit

[索引](../README.md) / [実装](../../../library/math/internal_bit.hpp)

## 機能・前提条件

整数のbit幅・2冪切上げ・末尾零・popcountなどの移植用補助。主に内部向けで型制約を持つ。

## インターフェース

```cpp
#include "library/math/internal_bit.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
defined(_M_X64));
using endian = std::endian;
enum class endian;
defined(_WIN32) || defined(__LITTLE_ENDIAN__);
defined(__BIG_ENDIAN__);
template <class To, class From>
template <class T>
struct remove_cvref;
template <class T>
struct is_unsigned_integer;
template <class T>
struct is_nonbool_integer;
template <class T>
using enable_unsigned =
template <class T>
using enable_integer =
template <class T>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int bit_width_fallback(T x) noexcept;
template <class T>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int popcount_fallback(T x) noexcept;
template <class T>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_reverse_fallback(T x) noexcept;
template <class T>
template <class T>
template <class T>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 bool is_negative(T value) noexcept;
template <class S>
shift_magnitude(S shift) noexcept;
template <class T, class S>
template <class T, class S>
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int countl_zero(T x) noexcept;
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int countl_one(T x) noexcept;
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int countr_zero(T x) noexcept;
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int countr_one(T x) noexcept;
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int popcount(T x) noexcept;
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 bool has_single_bit(T x) noexcept;
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int bit_width(T x) noexcept;
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_floor(T x) noexcept;
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_ceil(T x);
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T rotl(T x, int shift) noexcept;
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T rotr(T x, int shift) noexcept;
template <class T, internal_bit_detail::enable_integer<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T byteswap(T value) noexcept;
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_reverse(T x) noexcept;
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_repeat(T x, int length);
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_compress(T x, T mask) noexcept;
template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_expand(T x, T mask) noexcept;
template <class T, class S,
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T shl(T x, S shift) noexcept;
template <class T, class S,
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T shr(T x, S shift) noexcept;
```

## 計算量

固定幅整数で O(1)。fallbackは整数bit幅に比例する場合がある。空間 O(1)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

直接includeするverifyはありません。依存先からの間接利用は `make report` で確認できます。
