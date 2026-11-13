# static_bit_vector

[索引](../README.md) / [実装](../../../library/data_structure/static_bit_vector.hpp)

## 機能・前提条件

固定bit列とprefix rank。setの後にinitを呼んで累積個数を確定する。rank1(i) は [0,i) の1の数。

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

標準ライブラリのみ。

## Verify・利用例

直接includeするverifyはありません。依存先からの間接利用は `make report` で確認できます。
