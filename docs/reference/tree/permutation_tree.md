# permutation_tree

[索引](../README.md) / [実装](../../../library/tree/permutation_tree.hpp)

## 機能・前提条件

順列の共通区間を表す木を構築する。順列の値の条件、ノード区間と型は公開Nodeとverifyを参照。

## インターフェース

```cpp
#include "library/tree/permutation_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct PermutationTree;
struct StaticRMQ;
StaticRMQ();
StaticRMQ(vector<int> v);
int naive_prod(int l, int r);
int prod(int l, int r);
enum NodeType;
struct Node;
PermutationTree();
template<class INT> PermutationTree(const vector<INT>& p);
template<class INT> void build(vector<INT> p);
```

## 計算量

O(N log N) 時間・空間を保守的上界とする。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/common_interval_decomposition_tree`](../../../verify/library_checker/tree/common_interval_decomposition_tree.cpp)
