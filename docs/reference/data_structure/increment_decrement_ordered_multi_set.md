# increment_decrement_ordered_multi_set

[索引](../README.md) / [実装](../../../library/data_structure/increment_decrement_ordered_multi_set.hpp)

## 機能・前提条件

各要素を±1して順位順を保つ有界整数多重集合。kth は値ではなく要素IDを返す。更新後も値域内に収まる必要がある。

## インターフェース

```cpp
#include "library/data_structure/increment_decrement_ordered_multi_set.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct IncrementDecrementOrderedMultiSet;
IncrementDecrementOrderedMultiSet(int n_, int m_);
IncrementDecrementOrderedMultiSet(int n_, int m_, int x);
void increment(int i);
void decrement(int i);
inline int kth(int x) const;
inline int count_lt(int x) const;
inline int count_interval(int x, int y) const;
```

## 計算量

構築 O(N+M)、更新・順位参照・個数問い合わせ O(1)、空間 O(N+M)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/static_range_mode_query_increment_decrement_ordered_multiset`](../../../verify/library_checker/data_structure/static_range_mode_query_increment_decrement_ordered_multiset.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
