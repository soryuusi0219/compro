# internal/treap_base

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/treap_base.hpp)

## 機能・前提条件

親付きimplicit treapの平衡化コア。固定した優先度による最大heapを維持します。Opsはpush/pull/size/local_sizeと、書き換え可能なpriority(node)を提供します。ノードの所有権は呼出側にあります。

## インターフェース

`TreapBase<Node,Ops>(uint64_t seed=時刻由来)`。0は1へ正規化します。prepare(node)で疑似乱数の32bit優先度を設定します。TreeAction用のExtraにはTreapPriorityを使用できます。

- access_node(root&,node)：探索済みのノードをアクセスする高速化用hook。splayではrootを更新し、treapではノードをそのまま返します。
- access(root&,k)：対象ノードを返します。splayと異なりrootを変更しません。
- split(root,k)：論理位置で分割。kはノード間の境界に限ります。複数要素ブロックの内部を切る操作は呼出側が実装します。
- join(left,right)：inorder順の2列を結合。返却する根の親はnull。
- build(n,factory)：prepare済みの新しいノードをinorder順に取得し、Cartesian treeとして線形構築します。
- bubble_up(node)：新規ノードを接続した後、回転して優先度順を復元し、根を返します。接続経路の集約を更新し、関連タグをpushした状態で呼んでください。

同優先度のheap順も許し、joinでは左側を優先します。入力する根同士はノードを共有せず、ppがnullである必要があります。

## 計算量

到達可能ノード数K。access/split/join/bubble_upは期待 O(log(K+1))、単発最悪 O(K)。再帰深さも木の高さに依存します。prepareは O(1)、buildは最悪 O(K) 時間・O(K) 作業空間。乱数の優先度による期待値であり、最悪対数時間の保証ではありません。

## 依存関係

- [binary_tree_ops.hpp](binary_tree_ops.md)
- [cartesian_tree_base.hpp](cartesian_tree_base.md)

## Verify・利用例

- [unit/balanced_trees](../../../../verify/unit/balanced_trees.cpp)
- [unit/pointer_structures](../../../../verify/unit/pointer_structures.cpp)
