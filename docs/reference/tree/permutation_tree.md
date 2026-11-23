# permutation_tree

[索引](../README.md) / [実装](../../../library/tree/permutation_tree.hpp)

## 機能・前提条件

順列の共通区間を表す木を構築する。入力は0始まりの順列。順列を末尾から取り除いて各値の直前・直後の値を記録し、区間を拡張できるかを判定する。

## インターフェース

```cpp
#include "library/tree/permutation_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct PermutationTree;
using StaticRMQ = soryuusi::StaticRMQ<int>;
StaticRMQ();
StaticRMQ(vector<int> v);
int naive_prod(int l, int r) const;
int prod(int l, int r) const;
enum NodeType;
struct Node;
PermutationTree();
template<class INT> PermutationTree(const vector<INT>& p);
template<class INT> void build(const vector<INT>& p);
```

## 計算量

木の構築は O(N) 時間・空間。各スタック要素を高々一度取り出し、確定した子の親を書き込む回数もノード数に比例する。

## 依存関係

- [static_rmq.hpp](../data_structure/static_rmq.md)（公開補助型 `StaticRMQ`）

## Verify・利用例

- [`library_checker/common_interval_decomposition_tree`](../../../verify/library_checker/tree/common_interval_decomposition_tree.cpp)
