# internal/splay_base

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/splay_base.hpp)

## 機能・前提条件

所有権を持たないsplayの平衡化コア。Nodeはpp/lp/rpを持ち、Opsがpush/pullを提供します。祖先を補助木の根まで集め、上からpushしてから回転します。LCTのpath-parentも維持します。

## インターフェース

`SplayBase<Node,Ops>` の基本操作は `splay(node)`。nullも許し、呼出後はnodeが補助木の根になります。

列用にはsize/local_sizeを持つOpsを渡し、次のBackendインターフェースを利用できます。

- prepare(node)：splayでは何もしません。
- access(root&,k)：k番目をsplayし、rootを更新して対象ノードを返します。1ノード1要素を要求します。
- split(root,k)：先頭k要素と残りの根。両方の親はnull。0<=k<=size(root)。
- join(left,right)：順序の保たれた2列を連結。入力は独立した木の根である必要があります。
- build(n,factory)：factory(i)からノードを取得し、平衡な木を直接構築します。

作業配列はインスタンス内で再利用します。コピーしたコアで可変ノードを共有する利用は避けてください。コア自体はノードを確保・解放しません。

## 計算量

splay/access/split/joinは償却 O(log(N+1))、単発最悪 O(N)。buildは O(N) 時間・O(log(N+1)) 再帰空間。祖先の作業配列は最大 O(N) で容量を再利用します。

## 依存関係

- [binary_tree_ops.hpp](binary_tree_ops.md)

## Verify・利用例

- [unit/balanced_trees](../../../../verify/unit/balanced_trees.cpp)
- [unit/pointer_structures](../../../../verify/unit/pointer_structures.cpp)
