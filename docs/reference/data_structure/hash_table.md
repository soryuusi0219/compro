# hash_table

[索引](../README.md) / [実装](../../../library/data_structure/hash_table.hpp)

## 機能・前提条件

固定容量の開番地ハッシュ表。キー型・値型・hashを指定する。容量上限を守り、hash は十分に分散するものを選ぶ。

## インターフェース

```cpp
#include "library/data_structure/hash_table.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class U, auto hash>
struct HashTable;
using u32 = uint32_t;
using u64 = uint64_t;
HashTable(int n);
bool contains(const T &k);
U get(const T &k, U defalt);
template<class T>
uint64_t hash_int(T x);
```

## 計算量

容量内・良好なhashで期待 O(1) のアクセス、最悪 O(C)、空間 O(C)。C は確保容量。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/associative_array`](../../../verify/library_checker/data_structure/associative_array.cpp)
- [`library_checker/static_range_count_distinct_offline`](../../../verify/library_checker/data_structure/static_range_count_distinct_offline.cpp)
- [`library_checker/discrete_logarithm_mod`](../../../verify/library_checker/number_theory/discrete_logarithm_mod.cpp)
