# linear_sieve

[索引](../README.md) / [実装](../../../library/math/linear_sieve.hpp)

## 機能・前提条件

最小素因数と素数一覧を線形篩で構築する。表の範囲内の整数を問い合わせる。

## インターフェース

```cpp
#include "library/math/linear_sieve.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct Linear_Sieve;
Linear_Sieve();
Linear_Sieve(int n);
inline int operator[](int n) const;
inline int smallest_prime_factor(int n) const;
inline bool is_prime(int n) const;
inline const std::vector<int>& spf_table() const;
inline const std::vector<int>& prime_table() const;
```

## 計算量

構築 O(N)、表の参照 O(1)、空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/enumerate_primes_linear_sieve`](../../../verify/library_checker/number_theory/enumerate_primes_linear_sieve.cpp)
