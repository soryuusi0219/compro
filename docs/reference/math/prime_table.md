# prime_table

[索引](../README.md) / [実装](../../../library/math/prime_table.hpp)

## 機能・前提条件

上限までの素数一覧を共有キャッシュで取得する。返却参照の寿命と後続呼出しによる再確保に注意する。

## インターフェース

```cpp
#include "library/math/prime_table.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
inline std::vector<int>& PrimeTable(int n);
```

## 計算量

篩の走査は上限Nに O(N log N) を上界とする。保持空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/enumerate_primes`](../../../verify/library_checker/number_theory/enumerate_primes.cpp)
- [`library_checker/enumerate_primes_linear_sieve`](../../../verify/library_checker/number_theory/enumerate_primes_linear_sieve.cpp)
