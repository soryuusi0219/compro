# cartesian_tree

[索引](../README.md) / [実装](../../../library/data_structure/cartesian_tree.hpp)

## 機能・前提条件

数列からCartesian treeを構築する。親配列・左右の子配列・根の tuple を返す。b は大小の向きを切り替える。

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

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/cartesian_tree`](../../../verify/library_checker/tree/cartesian_tree.cpp)
