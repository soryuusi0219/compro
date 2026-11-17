# cartesian_tree

[索引](../README.md) / [実装](../../../library/data_structure/cartesian_tree.hpp)

## 機能・前提条件

数列からCartesian treeを構築する。親配列・左右の子配列・根の tuple を返す。b=falseは最小値を根にする木（同値なら右側を優先）、b=trueは最大値を根にする木（同値なら左側を優先）です。空入力では各配列は空、根は互換性のため0を返します。内部の線形構築をtreapと共有しています。

## インターフェース

```cpp
#include "library/data_structure/cartesian_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
tuple<vector<int>,vector<int>,vector<int>,int> cartesian_tree(const vector<T>& v, bool b = false);
```

## 計算量

O(N) 時間・空間。

## 依存関係

- [internal/cartesian_tree_base.hpp](internal/cartesian_tree_base.md)

## Verify・利用例

- [unit/balanced_trees](../../../verify/unit/balanced_trees.cpp)

- [`library_checker/cartesian_tree`](../../../verify/library_checker/tree/cartesian_tree.cpp)
