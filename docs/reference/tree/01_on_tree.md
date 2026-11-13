# 01_on_tree

[索引](../README.md) / [実装](../../../library/tree/01_on_tree.hpp)

## 機能・前提条件

親より後に子を並べる制約の下で、c,dによる順序依存コストを最小化する木上順序付け。根の親は-1。

## インターフェース

```cpp
#include "library/tree/01_on_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
pair<T, vector<int>> solve01onTree(int n, vector<long long> par, vector<T> c, vector<T> d);
```

## 計算量

O(N log N) 時間、O(N) 空間。比較時の積を保持できる型を使う。

## 依存関係

- [dsu.hpp](../data_structure/dsu.md)

## Verify・利用例

- [`library_checker/rooted_tree_topological_order_with_minimum_inversions`](../../../verify/library_checker/tree/rooted_tree_topological_order_with_minimum_inversions.cpp)
