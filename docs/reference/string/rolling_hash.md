# rolling_hash

[索引](../README.md) / [実装](../../../library/string/rolling_hash.hpp)

## 機能・前提条件

2^61-1 modの部分列hash。複数base、連結、LCPなど。衝突を許す確率的比較なので完全な同一性判定ではない。

## インターフェース

```cpp
#include "library/string/rolling_hash.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct RolHashMint;
using u64 = uint64_t;
using u128 = __uint128_t;
using mint = RolHashMint;
static constexpr u64 take_mod(u64 x);
static constexpr u64 mul(u64 a, u64 b);
constexpr RolHashMint();
template<class T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
constexpr RolHashMint(T v_);
static constexpr mint raw(u64 v_);
constexpr u64 val() const;
constexpr mint& operator++();
constexpr mint& operator--();
constexpr mint operator++(int);
constexpr mint operator--(int);
constexpr mint& operator+=(const mint& rhs);
constexpr mint& operator-=(const mint& rhs);
constexpr mint& operator*=(const mint& rhs);
friend constexpr mint operator+(mint lhs, const mint& rhs);
friend constexpr mint operator-(mint lhs, const mint& rhs);
friend constexpr mint operator*(mint lhs, const mint& rhs);
friend constexpr bool operator==(const mint& lhs, const mint& rhs);
friend constexpr bool operator!=(const mint& lhs, const mint& rhs);
friend constexpr bool operator<(const mint& lhs, const mint& rhs);
friend constexpr bool operator>(const mint& lhs, const mint& rhs);
friend constexpr bool operator<=(const mint& lhs, const mint& rhs);
friend constexpr bool operator>=(const mint& lhs, const mint& rhs);
static constexpr mint muladd(const mint a, const mint b, const mint c);
static constexpr mint mulsub(const mint a, const mint b, const mint c);
static constexpr mint submul(const mint a, const mint b, const mint c);
template<int N>
struct RolHashValue;
using Val = RolHashValue;
using mint = RolHashMint;
constexpr RolHashValue();
template<class T>
constexpr RolHashValue(const T& x);
static constexpr Val raw(uint64_t x);
constexpr mint& operator[](int i);
constexpr const mint& operator[](int i) const;
constexpr Val& operator+=(const Val& rhs);
constexpr Val& operator-=(const Val& rhs);
constexpr Val& operator*=(const Val& rhs);
friend constexpr Val operator+(Val lhs, const Val& rhs);
friend constexpr Val operator-(Val lhs, const Val& rhs);
friend constexpr Val operator*(Val lhs, const Val& rhs);
friend constexpr bool operator==(const Val& lhs, const Val& rhs);
friend constexpr bool operator!=(const Val& lhs, const Val& rhs);
friend constexpr bool operator<(const Val& lhs, const Val& rhs);
friend constexpr bool operator>(const Val& lhs, const Val& rhs);
friend constexpr bool operator<=(const Val& lhs, const Val& rhs);
friend constexpr bool operator>=(const Val& lhs, const Val& rhs);
static constexpr Val muladd(const Val& a, const Val& b, const Val& c);
static constexpr Val mulsub(const Val& a, const Val& b, const Val& c);
static constexpr Val submul(const Val& a, const Val& b, const Val& c);
template<int N = 1, int id = -1>
struct RollingHash;
RollingHash();
template<class T>
explicit RollingHash(const T& v);
inline int size() const;
template<class T> void push_back(T x);
Hash get_hash(int l, int r) const;
bool is_same(int l1, int r1, int l2, int r2) const;
bool is_same(int l1, int r1, int l2, int r2, const RollingHash& y) const;
int lcp(int l1, int l2) const;
int lcp(int l1, int l2, const RollingHash& y) const;
```

## 計算量

base数B、構築 O(NB)、部分hash・連結 O(B)、二分探索LCP O(B log N)、空間 O(NB)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/z_algorithm_rolling_hash`](../../../verify/library_checker/string/z_algorithm_rolling_hash.cpp)
- [`unit/rolling_hash`](../../../verify/unit/rolling_hash.cpp)
