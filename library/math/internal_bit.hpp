// https://github.com/soryuusi0219/compro/blob/main/library/math/internal_bit.hpp
#pragma once

#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

#if defined(_MSVC_LANG)
#define SORYUUSI_INTERNAL_BIT_CPLUSPLUS _MSVC_LANG
#else
#define SORYUUSI_INTERNAL_BIT_CPLUSPLUS __cplusplus
#endif

#if SORYUUSI_INTERNAL_BIT_CPLUSPLUS >= 202002L && defined(__has_include)
#if __has_include(<bit>)
#include <bit>
#endif
#elif SORYUUSI_INTERNAL_BIT_CPLUSPLUS >= 202002L
#include <bit>
#endif

#if defined(__BMI2__) &&                                                    \
    (defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) ||       \
     defined(_M_X64))
#include <immintrin.h>
#define SORYUUSI_INTERNAL_BIT_HAS_BMI2 1
#else
#define SORYUUSI_INTERNAL_BIT_HAS_BMI2 0
#endif

#ifndef __has_builtin
#define __has_builtin(x) 0
#define SORYUUSI_INTERNAL_BIT_UNDEF_HAS_BUILTIN
#endif

#if SORYUUSI_INTERNAL_BIT_CPLUSPLUS >= 201402L
#define SORYUUSI_INTERNAL_BIT_CONSTEXPR14 constexpr
#else
#define SORYUUSI_INTERNAL_BIT_CONSTEXPR14 inline
#endif

#if defined(__cpp_lib_bit_cast) && __cpp_lib_bit_cast >= 201806L
#define SORYUUSI_INTERNAL_BIT_CONSTEXPR_BIT_CAST constexpr
#define SORYUUSI_INTERNAL_BIT_USE_STD_BIT_CAST 1
#elif __has_builtin(__builtin_bit_cast)
#define SORYUUSI_INTERNAL_BIT_CONSTEXPR_BIT_CAST constexpr
#define SORYUUSI_INTERNAL_BIT_USE_BUILTIN_BIT_CAST 1
#else
#define SORYUUSI_INTERNAL_BIT_CONSTEXPR_BIT_CAST inline
#endif

namespace soryuusi {

// Compatibility layer for the standard <bit> interface. As in the standard,
// bit-counting, power-of-two, rotation, and permutation functions accept
// unsigned integer types. Implementations support widths through 64 bits.

#if defined(__cpp_lib_endian) && __cpp_lib_endian >= 201907L
using endian = std::endian;
#else
enum class endian {
  little = 0x1234,
  big = 0x4321,
#if (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) &&          \
     __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) ||                         \
    defined(_WIN32) || defined(__LITTLE_ENDIAN__)
  native = little
#elif (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) &&          \
       __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) ||                          \
    defined(__BIG_ENDIAN__)
  native = big
#else
  native = 0
#endif
};
#endif

template <class To, class From>
SORYUUSI_INTERNAL_BIT_CONSTEXPR_BIT_CAST To bit_cast(
    const From& from) noexcept {
  static_assert(sizeof(To) == sizeof(From),
                "bit_cast requires source and destination of equal size");
  static_assert(std::is_trivially_copyable<To>::value,
                "bit_cast destination must be trivially copyable");
  static_assert(std::is_trivially_copyable<From>::value,
                "bit_cast source must be trivially copyable");
#if defined(SORYUUSI_INTERNAL_BIT_USE_STD_BIT_CAST)
  return std::bit_cast<To>(from);
#elif defined(SORYUUSI_INTERNAL_BIT_USE_BUILTIN_BIT_CAST)
  return __builtin_bit_cast(To, from);
#else
  To result;
  std::memcpy(&result, &from, sizeof(To));
  return result;
#endif
}

namespace internal_bit_detail {

template <class T>
struct remove_cvref {
  typedef typename std::remove_cv<
      typename std::remove_reference<T>::type>::type type;
};

template <class T>
struct is_unsigned_integer
    : std::integral_constant<
          bool,
          std::is_integral<typename remove_cvref<T>::type>::value &&
              std::is_unsigned<typename remove_cvref<T>::type>::value &&
              !std::is_same<typename remove_cvref<T>::type, bool>::value> {};

template <class T>
struct is_nonbool_integer
    : std::integral_constant<
          bool,
          std::is_integral<typename remove_cvref<T>::type>::value &&
              !std::is_same<typename remove_cvref<T>::type, bool>::value> {};

template <class T>
using enable_unsigned =
    typename std::enable_if<is_unsigned_integer<T>::value, int>::type;

template <class T>
using enable_integer =
    typename std::enable_if<is_nonbool_integer<T>::value, int>::type;

template <class T>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int bit_width_fallback(T x) noexcept {
  static_assert(std::numeric_limits<T>::digits <= 64,
                "internal_bit supports integer types up to 64 bits");
  std::uint64_t value = static_cast<std::uint64_t>(x);
  int result = 0;
  if (value >> 32) value >>= 32, result += 32;
  if (value >> 16) value >>= 16, result += 16;
  if (value >> 8) value >>= 8, result += 8;
  if (value >> 4) value >>= 4, result += 4;
  if (value >> 2) value >>= 2, result += 2;
  if (value >> 1) result += 1;
  return result + static_cast<int>(value != 0);
}

template <class T>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int popcount_fallback(T x) noexcept {
  static_assert(std::numeric_limits<T>::digits <= 64,
                "internal_bit supports integer types up to 64 bits");
  std::uint64_t value = static_cast<std::uint64_t>(x);
  value -= (value >> 1) & UINT64_C(0x5555555555555555);
  value = (value & UINT64_C(0x3333333333333333)) +
          ((value >> 2) & UINT64_C(0x3333333333333333));
  value = (value + (value >> 4)) & UINT64_C(0x0f0f0f0f0f0f0f0f);
  return static_cast<int>((value * UINT64_C(0x0101010101010101)) >> 56);
}

template <class T>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_reverse_fallback(T x) noexcept {
  static_assert(std::numeric_limits<T>::digits <= 64,
                "internal_bit supports integer types up to 64 bits");
  const int digits = std::numeric_limits<T>::digits;
  if ((digits & (digits - 1)) != 0) {
    T result = 0;
    for (int index = 0; index < digits; ++index) {
      result = static_cast<T>(static_cast<T>(result << 1) | (x & T(1)));
      x = static_cast<T>(x >> 1);
    }
    return result;
  }
  T mask = static_cast<T>(~T(0));
  for (int shift = digits >> 1; shift != 0; shift >>= 1) {
    mask = static_cast<T>(mask ^ static_cast<T>(mask << shift));
    x = static_cast<T>(((x >> shift) & mask) |
                       (static_cast<T>(x << shift) & static_cast<T>(~mask)));
  }
  return x;
}

template <class T>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_compress_fallback(T x,
                                                          T mask) noexcept {
  static_assert(std::numeric_limits<T>::digits <= 64,
                "internal_bit supports integer types up to 64 bits");
  if ((std::numeric_limits<T>::digits &
       (std::numeric_limits<T>::digits - 1)) != 0) {
    T result = 0;
    T output_bit = 1;
    while (mask != 0) {
      const T input_bit = static_cast<T>(mask & static_cast<T>(-mask));
      if ((x & input_bit) != 0) result = static_cast<T>(result | output_bit);
      mask = static_cast<T>(mask & static_cast<T>(mask - 1));
      output_bit = static_cast<T>(output_bit << 1);
    }
    return result;
  }
  x = static_cast<T>(x & mask);
  T moving_mask = static_cast<T>(static_cast<T>(~mask) << 1);
  for (int shift = 1; shift < std::numeric_limits<T>::digits; shift <<= 1) {
    T prefix = static_cast<T>(moving_mask ^
                              static_cast<T>(moving_mask << 1));
    for (int distance = 2; distance < std::numeric_limits<T>::digits;
         distance <<= 1) {
      prefix = static_cast<T>(prefix ^ static_cast<T>(prefix << distance));
    }
    const T move = static_cast<T>(prefix & mask);
    mask = static_cast<T>((mask ^ move) | (move >> shift));
    const T selected = static_cast<T>(x & move);
    x = static_cast<T>((x ^ selected) | (selected >> shift));
    moving_mask = static_cast<T>(moving_mask & static_cast<T>(~prefix));
  }
  return x;
}

template <class T>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_expand_fallback(T x,
                                                        T mask) noexcept {
  static_assert(std::numeric_limits<T>::digits <= 64,
                "internal_bit supports integer types up to 64 bits");
  if ((std::numeric_limits<T>::digits &
       (std::numeric_limits<T>::digits - 1)) != 0) {
    T result = 0;
    T input_bit = 1;
    while (mask != 0) {
      const T output_bit = static_cast<T>(mask & static_cast<T>(-mask));
      if ((x & input_bit) != 0) result = static_cast<T>(result | output_bit);
      mask = static_cast<T>(mask & static_cast<T>(mask - 1));
      input_bit = static_cast<T>(input_bit << 1);
    }
    return result;
  }
  const int levels = bit_width_fallback(
      static_cast<unsigned>(std::numeric_limits<T>::digits - 1));
  T moves[6] = {};
  const T original_mask = mask;
  T moving_mask = static_cast<T>(static_cast<T>(~mask) << 1);
  int level = 0;
  for (int shift = 1; shift < std::numeric_limits<T>::digits;
       shift <<= 1, ++level) {
    T prefix = static_cast<T>(moving_mask ^
                              static_cast<T>(moving_mask << 1));
    for (int distance = 2; distance < std::numeric_limits<T>::digits;
         distance <<= 1) {
      prefix = static_cast<T>(prefix ^ static_cast<T>(prefix << distance));
    }
    moves[level] = static_cast<T>(prefix & mask);
    mask = static_cast<T>((mask ^ moves[level]) | (moves[level] >> shift));
    moving_mask = static_cast<T>(moving_mask & static_cast<T>(~prefix));
  }
  x = static_cast<T>(x & mask);
  for (int index = levels; index-- > 0;) {
    const int shift = 1 << index;
    const T move = moves[index];
    x = static_cast<T>((x & static_cast<T>(~(move >> shift))) |
                       (static_cast<T>(x << shift) & move));
  }
  return static_cast<T>(x & original_mask);
}

template <class T>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 bool is_negative(T value) noexcept {
  return std::is_signed<T>::value && value < 0;
}

template <class S>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 typename std::make_unsigned<S>::type
shift_magnitude(S shift) noexcept {
  typedef typename std::make_unsigned<S>::type U;
  const U value = static_cast<U>(shift);
  return is_negative(shift) ? static_cast<U>(U(0) - value) : value;
}

template <class T, class S>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T shift_left_positive(T x,
                                                        S shift) noexcept {
  typedef typename std::make_unsigned<T>::type U;
  const int digits = std::numeric_limits<U>::digits;
  if (shift >= static_cast<S>(digits)) return T(0);
  return static_cast<T>(static_cast<U>(x) << static_cast<int>(shift));
}

template <class T, class S>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T shift_right_positive(T x,
                                                         S shift) noexcept {
  typedef typename std::make_unsigned<T>::type U;
  const int digits = std::numeric_limits<U>::digits;
  const bool negative = is_negative(x);
  if (shift >= static_cast<S>(digits)) return negative ? T(-1) : T(0);
  const int amount = static_cast<int>(shift);
  U result = static_cast<U>(x) >> amount;
  if (negative && amount != 0) {
    result = static_cast<U>(result |
                            static_cast<U>(static_cast<U>(~U(0))
                                           << (digits - amount)));
  }
  return static_cast<T>(result);
}

}  // namespace internal_bit_detail

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int countl_zero(T x) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
  return std::countl_zero(x);
#elif __has_builtin(__builtin_clz) || defined(__GNUC__)
  const int digits = std::numeric_limits<T>::digits;
  if (x == 0) return digits;
  if (sizeof(T) <= sizeof(unsigned int)) {
    return __builtin_clz(static_cast<unsigned int>(x)) -
           (std::numeric_limits<unsigned int>::digits - digits);
  }
  return __builtin_clzll(static_cast<unsigned long long>(x)) -
         (std::numeric_limits<unsigned long long>::digits - digits);
#else
  return std::numeric_limits<T>::digits -
         internal_bit_detail::bit_width_fallback(x);
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int countl_one(T x) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
  return std::countl_one(x);
#else
  return soryuusi::countl_zero(static_cast<T>(~x));
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int countr_zero(T x) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
  return std::countr_zero(x);
#elif __has_builtin(__builtin_ctz) || defined(__GNUC__)
  if (x == 0) return std::numeric_limits<T>::digits;
  if (sizeof(T) <= sizeof(unsigned int)) {
    return __builtin_ctz(static_cast<unsigned int>(x));
  }
  return __builtin_ctzll(static_cast<unsigned long long>(x));
#else
  return internal_bit_detail::bit_width_fallback(
      static_cast<T>((x & static_cast<T>(-x)) - 1));
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int countr_one(T x) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
  return std::countr_one(x);
#else
  return soryuusi::countr_zero(static_cast<T>(~x));
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int popcount(T x) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
  return std::popcount(x);
#elif __has_builtin(__builtin_popcount) || defined(__GNUC__)
  if (sizeof(T) <= sizeof(unsigned int)) {
    return __builtin_popcount(static_cast<unsigned int>(x));
  }
  return __builtin_popcountll(static_cast<unsigned long long>(x));
#else
  return internal_bit_detail::popcount_fallback(x);
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 bool has_single_bit(T x) noexcept {
#if defined(__cpp_lib_int_pow2) && __cpp_lib_int_pow2 >= 202002L
  return std::has_single_bit(x);
#else
  return x != 0 && (x & static_cast<T>(x - 1)) == 0;
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 int bit_width(T x) noexcept {
#if defined(__cpp_lib_int_pow2) && __cpp_lib_int_pow2 >= 202002L
  return std::bit_width(x);
#else
  return internal_bit_detail::bit_width_fallback(x);
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_floor(T x) noexcept {
#if defined(__cpp_lib_int_pow2) && __cpp_lib_int_pow2 >= 202002L
  return std::bit_floor(x);
#else
  return x == 0 ? T(0) : static_cast<T>(T(1) << (bit_width(x) - 1));
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_ceil(T x) {
#if defined(__cpp_lib_int_pow2) && __cpp_lib_int_pow2 >= 202002L
  return std::bit_ceil(x);
#else
  return x <= 1 ? T(1)
                : static_cast<T>(T(1) << bit_width(static_cast<T>(x - 1)));
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T rotl(T x, int shift) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
  return std::rotl(x, shift);
#else
  const int digits = std::numeric_limits<T>::digits;
  int amount = shift % digits;
  if (amount < 0) amount += digits;
  return amount == 0
             ? x
             : static_cast<T>(static_cast<T>(x << amount) |
                              static_cast<T>(x >> (digits - amount)));
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T rotr(T x, int shift) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
  return std::rotr(x, shift);
#else
  const int digits = std::numeric_limits<T>::digits;
  int amount = shift % digits;
  if (amount < 0) amount += digits;
  return amount == 0
             ? x
             : static_cast<T>(static_cast<T>(x >> amount) |
                              static_cast<T>(x << (digits - amount)));
#endif
}

template <class T, internal_bit_detail::enable_integer<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T byteswap(T value) noexcept {
#if defined(__cpp_lib_byteswap) && __cpp_lib_byteswap >= 202110L
  return std::byteswap(value);
#else
  typedef typename std::make_unsigned<T>::type U;
  U x = static_cast<U>(value);
#if __has_builtin(__builtin_bswap16) || defined(__GNUC__)
  if (sizeof(T) == 1) return value;
  if (sizeof(T) == 2) {
    return static_cast<T>(__builtin_bswap16(static_cast<std::uint16_t>(x)));
  }
  if (sizeof(T) == 4) {
    return static_cast<T>(__builtin_bswap32(static_cast<std::uint32_t>(x)));
  }
  if (sizeof(T) == 8) {
    return static_cast<T>(__builtin_bswap64(static_cast<std::uint64_t>(x)));
  }
#endif
  U result = 0;
  for (std::size_t index = 0; index < sizeof(T); ++index) {
    result = static_cast<U>(static_cast<U>(result << CHAR_BIT) |
                            static_cast<U>(x & UCHAR_MAX));
    x = static_cast<U>(x >> CHAR_BIT);
  }
  return static_cast<T>(result);
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_reverse(T x) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 202607L
  return std::bit_reverse(x);
#elif __has_builtin(__builtin_bitreverse32)
  if (sizeof(T) <= sizeof(std::uint32_t)) {
    const std::uint32_t reversed =
        __builtin_bitreverse32(static_cast<std::uint32_t>(x));
    return static_cast<T>(reversed >>
                          (32 - std::numeric_limits<T>::digits));
  }
  return static_cast<T>(
      __builtin_bitreverse64(static_cast<std::uint64_t>(x)));
#else
  return internal_bit_detail::bit_reverse_fallback(x);
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_repeat(T x, int length) {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 202607L
  return std::bit_repeat(x, length);
#else
  assert(length > 0);
  const int digits = std::numeric_limits<T>::digits;
  if (length >= digits) return x;
  x = static_cast<T>(
      x & static_cast<T>(static_cast<T>(~T(0)) >> (digits - length)));
  int filled = length;
  while (filled < digits) {
    x = static_cast<T>(x | static_cast<T>(x << filled));
    if (filled >= digits - filled) break;
    filled <<= 1;
  }
  return x;
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_compress(T x, T mask) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 202607L
  return std::bit_compress(x, mask);
#elif SORYUUSI_INTERNAL_BIT_HAS_BMI2 &&                                   \
    (SORYUUSI_INTERNAL_BIT_CPLUSPLUS < 201402L ||                         \
     __has_builtin(__builtin_is_constant_evaluated))
#if SORYUUSI_INTERNAL_BIT_CPLUSPLUS >= 201402L
  if (!__builtin_is_constant_evaluated()) {
#endif
    if (sizeof(T) <= sizeof(std::uint32_t)) {
      return static_cast<T>(_pext_u32(static_cast<std::uint32_t>(x),
                                      static_cast<std::uint32_t>(mask)));
    }
    return static_cast<T>(_pext_u64(static_cast<std::uint64_t>(x),
                                    static_cast<std::uint64_t>(mask)));
#if SORYUUSI_INTERNAL_BIT_CPLUSPLUS >= 201402L
  }
  return internal_bit_detail::bit_compress_fallback(x, mask);
#endif
#else
  return internal_bit_detail::bit_compress_fallback(x, mask);
#endif
}

template <class T, internal_bit_detail::enable_unsigned<T> = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T bit_expand(T x, T mask) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 202607L
  return std::bit_expand(x, mask);
#elif SORYUUSI_INTERNAL_BIT_HAS_BMI2 &&                                   \
    (SORYUUSI_INTERNAL_BIT_CPLUSPLUS < 201402L ||                         \
     __has_builtin(__builtin_is_constant_evaluated))
#if SORYUUSI_INTERNAL_BIT_CPLUSPLUS >= 201402L
  if (!__builtin_is_constant_evaluated()) {
#endif
    if (sizeof(T) <= sizeof(std::uint32_t)) {
      return static_cast<T>(_pdep_u32(static_cast<std::uint32_t>(x),
                                      static_cast<std::uint32_t>(mask)));
    }
    return static_cast<T>(_pdep_u64(static_cast<std::uint64_t>(x),
                                    static_cast<std::uint64_t>(mask)));
#if SORYUUSI_INTERNAL_BIT_CPLUSPLUS >= 201402L
  }
  return internal_bit_detail::bit_expand_fallback(x, mask);
#endif
#else
  return internal_bit_detail::bit_expand_fallback(x, mask);
#endif
}

template <class T, class S,
          typename std::enable_if<
              internal_bit_detail::is_nonbool_integer<T>::value &&
                  internal_bit_detail::is_nonbool_integer<S>::value,
              int>::type = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T shl(T x, S shift) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 202607L
  return std::shl(x, shift);
#else
  typedef typename std::make_unsigned<S>::type US;
  const US magnitude = internal_bit_detail::shift_magnitude(shift);
  return internal_bit_detail::is_negative(shift)
             ? internal_bit_detail::shift_right_positive(x, magnitude)
             : internal_bit_detail::shift_left_positive(x, magnitude);
#endif
}

template <class T, class S,
          typename std::enable_if<
              internal_bit_detail::is_nonbool_integer<T>::value &&
                  internal_bit_detail::is_nonbool_integer<S>::value,
              int>::type = 0>
SORYUUSI_INTERNAL_BIT_CONSTEXPR14 T shr(T x, S shift) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 202607L
  return std::shr(x, shift);
#else
  typedef typename std::make_unsigned<S>::type US;
  const US magnitude = internal_bit_detail::shift_magnitude(shift);
  return internal_bit_detail::is_negative(shift)
             ? internal_bit_detail::shift_left_positive(x, magnitude)
             : internal_bit_detail::shift_right_positive(x, magnitude);
#endif
}

}  // namespace soryuusi

#undef SORYUUSI_INTERNAL_BIT_CONSTEXPR14
#undef SORYUUSI_INTERNAL_BIT_CONSTEXPR_BIT_CAST
#undef SORYUUSI_INTERNAL_BIT_CPLUSPLUS
#undef SORYUUSI_INTERNAL_BIT_HAS_BMI2
#undef SORYUUSI_INTERNAL_BIT_USE_STD_BIT_CAST
#undef SORYUUSI_INTERNAL_BIT_USE_BUILTIN_BIT_CAST

#ifdef SORYUUSI_INTERNAL_BIT_UNDEF_HAS_BUILTIN
#undef __has_builtin
#undef SORYUUSI_INTERNAL_BIT_UNDEF_HAS_BUILTIN
#endif
