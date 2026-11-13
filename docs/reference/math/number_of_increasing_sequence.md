# number_of_increasing_sequence

[索引](../README.md) / [実装](../../../library/math/number_of_increasing_sequence.hpp)

## 機能・前提条件

各位置の上限制約下の増加列を畳み込みDPで数える。n,mと上限列aを与える。階乗逆元が必要。

## インターフェース

```cpp
#include "library/math/number_of_increasing_sequence.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class mint>
mint num_inc_seq_under(long long n, long long m, vector<long long> a);
```

## 計算量

L=n+m、畳み込みが O(L log L) の範囲で O(L log² L) を上界とする。空間 O(L)。

## 依存関係

- [convolution.hpp](convolution.md)
- [modint_factorials.hpp](modint_factorials.md)

## Verify・利用例

- [`unit/math_misc`](../../../verify/unit/math_misc.cpp)
