# hash_table

[索引](../README.md) / [実装](../../../library/data_structure/hash_table.hpp)

## 機能・前提条件

固定容量の開番地ハッシュ表。キー型・値型・ハッシュ関数を指定する。等価なキーは同じハッシュ値を返す必要がある。`n >= 0` は想定する登録件数で、確保容量 `C` は `max(1,2*n)` 以上の最小の2冪。

`operator[]` は未登録キーを `U{}` で追加し、格納値への参照を返す。既存の参照は別のキーの挿入で無効化されない。`contains` と `get` は検索のみを行う。満杯の表でも検索は1周以内に終わり、未登録キーの挿入は `std::length_error` を投げる。

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
U &operator[](const T &k);
bool contains(const T &k);
U get(const T &k, U defalt);
template<class T>
uint64_t hash_int(T x);
```

## 計算量

負荷率が一定以下でハッシュが十分に分散するとき、アクセスは期待 `O(1)`。同じハッシュ値に集中した場合は最悪 `O(C)`。構築・空間は `O(C)`。

## 依存関係

- [hash_table_storage.hpp](internal/hash_table_storage.md)

## Verify・利用例

- [`unit/unionfind_hash`](../../../verify/unit/unionfind_hash.cpp)
- [`library_checker/associative_array`](../../../verify/library_checker/data_structure/associative_array.cpp)
- [`library_checker/static_range_count_distinct_offline`](../../../verify/library_checker/data_structure/static_range_count_distinct_offline.cpp)
- [`library_checker/discrete_logarithm_mod`](../../../verify/library_checker/number_theory/discrete_logarithm_mod.cpp)
