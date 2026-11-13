# 64_ary_tree

[索引](../README.md) / [実装](../../../library/utility/64_ary_tree.hpp)

## 機能・前提条件

有界整数集合を64分木bit列で保持する。insert/erase/contains/next/prev。範囲外の検索結果の番兵を確認する。

`next(x)` はx以上の最小要素（なければn）、`prev(x)` はx未満の最大要素（なければ-1）です。後者は負の番兵を扱える符号付き型で使用します。

## インターフェース

```cpp
#include "library/utility/64_ary_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct BitSetSet;
using u64 = uint64_t;
BitSetSet() = default;
BitSetSet(T n_);
void insert(T x);
void erase(T x);
bool operator[](T x) const;
bool contains(T x);
T next(T x);
T prev(T x);
```

## 計算量

更新・前後探索 O(log₆₄ U)、contains O(1)、空間 O(U/64) word。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/predecessor_problem`](../../../verify/library_checker/data_structure/predecessor_problem.cpp)
