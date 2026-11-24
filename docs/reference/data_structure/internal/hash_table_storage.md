# hash_table_storage

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/hash_table_storage.hpp)

## 機能・前提条件

固定容量の開番地ハッシュ表で共有する内部ストレージ。容量 `C` は `max(1,2*n)` 以上の最小の2冪。キーと値の配列から空き判定用の1バイトを分け、使用中のスロットにはハッシュ値の一部を記録する。

## インターフェース

```cpp
template<class K, class V, auto hash, bool DoubleHash = true> class HashTableStorage;
explicit HashTableStorage(int n);
std::size_t lookup(const K& k);
std::size_t insert(const K& k, auto initial);
std::size_t end() const;
decltype(auto) key(std::size_t i) const;
decltype(auto) value(std::size_t i);
```

`lookup` は該当スロットの番号を返し、見つからないときは `end()` を返す。`insert` は未登録キーに対してだけ `initial()` を呼ぶ。満杯の状態で未登録キーを挿入すると `std::length_error` を投げる。テーブルは固定容量で、挿入済みのスロット番号は変わらない。

`DoubleHash = true` はハッシュの上位側から奇数の探索幅を作り、`false` は隣接するスロットを順に探す。どちらも1周で探索を打ち切る。等価なキーのハッシュ値は一致している必要がある。

## 計算量

構築・空間は `O(C)`。十分に分散するハッシュと一定以下の負荷率では一件のアクセスは期待 `O(1)`、衝突が集中した場合は最悪 `O(C)`。

## 依存関係

- [internal_bit.hpp](../../math/internal/internal_bit.md)

## Verify・利用例

- [`library_checker/associative_array`](../../../../verify/library_checker/data_structure/associative_array.cpp)
- [`library_checker/unionfind_dynamic`](../../../../verify/library_checker/data_structure/unionfind_dynamic.cpp)
- [`unit/unionfind_hash`](../../../../verify/unit/unionfind_hash.cpp)
