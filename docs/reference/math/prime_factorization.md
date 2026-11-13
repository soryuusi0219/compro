# prime_factorization

[索引](../README.md) / [実装](../../../library/math/prime_factorization.hpp)

## 機能・前提条件

64bit整数のMiller–RabinとPollard rhoによる素因数分解・約数列挙。入力は正整数。

## インターフェース

```cpp
#include "library/math/prime_factorization.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
inline std::uint64_t pollard_rho_wide(std::uint64_t n);
inline std::uint64_t pollard_rho(std::uint64_t n);
inline void factorize(std::uint64_t n, std::vector<std::uint64_t>& factors);
inline long long Pollard_Rho(long long n);
inline std::vector<long long> prime_factorization(long long n);
inline std::vector<long long> divisors(long long n);
```

## 計算量

rhoは確率的で最小素因数pに期待 O(√p) 回程度のmod演算。約数列挙は約数個数Dとソート O(D log D)。

## 依存関係

- [math.hpp](math.md)

## Verify・利用例

- [`library_checker/factorize`](../../../verify/library_checker/number_theory/factorize.cpp)
- [`library_checker/primality_test`](../../../verify/library_checker/number_theory/primality_test.cpp)
