# static_bit_vector

[索引](../README.md) / [実装](../../../library/data_structure/static_bit_vector.hpp)

## 機能・前提条件

固定bit列とprefix rank。setの後にinitを呼んで累積個数を確定する。rank1(i) は [0,i) の1の数。

`rank1(i)` と `rank0(i)` は `0 <= i <= N` で使える。64の倍数の末尾や空列も扱う。ワード内のビット数には `internal_bit.hpp` の移植用実装を使う。

## インターフェース

```cpp
#include "library/data_structure/static_bit_vector.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct StaticBitVector;
using u64 = std::uint64_t;
StaticBitVector() = default;
explicit StaticBitVector(int n);
void build(int n);
void set(int i);
void init();
bool get(int i) const;
int rank1(int i) const;
int rank0(int i) const;
```

## 計算量

構築・init O(N/64)、get/rank O(1)、空間 O(N/64) word。

## 依存関係

- [internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [`unit/static_rank_and_order_statistics`](../../../verify/unit/static_rank_and_order_statistics.cpp)
直接includeするverifyはありません。依存先からの間接利用は `make report` で確認できます。
