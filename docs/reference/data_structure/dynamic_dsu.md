# dynamic_dsu

[索引](../README.md) / [実装](../../../library/data_structure/dynamic_dsu.hpp)

## 機能・前提条件

疎な頂点IDをハッシュ表で管理するUnion-Find。負の値を含む `int` を頂点IDとして使える。操作で初めて参照した頂点はサイズ1で登録される。コンストラクタには操作で参照する異なる頂点数の上限を渡す。

## インターフェース

```cpp
#include "library/data_structure/dynamic_dsu.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct dynamic_dsu;
dynamic_dsu(int n);
int leader(int u);
bool same(int u, int v);
int size(int u);
int merge(int u, int v);
```

## 計算量

各端点をハッシュ表で引いた後、親はスロット番号でたどる。良好なハッシュを仮定すると通常操作は期待償却 `O(α(N))`。衝突が集中した場合は1操作で最悪 `O(C + log(N+1))`。ここで `N` は登録済み頂点数、`C` は確保容量。構築と空間は `O(C)`。

## 依存関係

- [hash_table.hpp](hash_table.md)
- [hash_table_storage.hpp](internal/hash_table_storage.md)

## Verify・利用例

- [`unit/unionfind_hash`](../../../verify/unit/unionfind_hash.cpp)
- [`library_checker/unionfind_dynamic`](../../../verify/library_checker/data_structure/unionfind_dynamic.cpp)
