# internal/splay_base

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/splay_base.hpp)

## 機能・前提条件

所有権を持たないsplayの平衡化コア。Nodeはpp/lp/rpを持ち、Opsがpush/pullを提供します。祖先を補助木の根まで集め、上からpushしてから回転します。対象が既に補助木の根なら、そのノードをpushするだけです。LCTのpath-parentも維持します。

## インターフェース

`SplayBase<Node,Ops>` の基本操作は `splay(node)`。nullも許し、呼出後はnodeが補助木の根になります。

列用にはsize/local_sizeを持つOpsを渡し、次のBackendインターフェースを利用できます。

- prepare(node)：splayでは何もしません。
- access_node(root&,node)：探索済みのノードをアクセスする高速化用hook。splayではrootを更新し、treapではノードをそのまま返します。
- access(root&,k)：k番目をsplayし、rootを更新して対象ノードを返します。1ノード1要素を要求します。
- split(root,k)：先頭k要素と残りの根。両方の親はnull。0<=k<=size(root)。
- join(left,right)：順序の保たれた2列を連結。入力は独立した木の根である必要があります。
- build(n,factory)：factory(i)からノードを取得し、平衡な木を直接構築します。

作業配列はインスタンス内で再利用します。コピーしたコアで可変ノードを共有する利用は避けてください。コア自体はノードを確保・解放しません。

順位・キー探索が根からpushした経路では `access_pushed(root,node)` を使います。
`with_range(root,l,r,f)` は1ノード1要素の木で、範囲の直前・直後をsplayし、その間の部分木を渡します。
読み取りでは `with_range<false>` で境界の再集約を省きます。
`split_prefix` はinorder順の接頭辞を判定する述語で分割します。
`split_weighted` は必要ならブロックを切るcallbackを呼び、`fold_range` は境界の部分ブロックだけをcallbackで集約します。
単要素の挿入は分割結果を新ノードに取り付け、削除は対象を根にして左右をjoinします。

## 計算量

splay/access/split/joinは償却 O(log(N+1))、単発最悪 O(N)。buildは O(N) 時間・O(log(N+1)) 再帰空間。祖先の作業配列は最大 O(N) で容量を再利用します。

## 依存関係

- [binary_tree_ops.hpp](binary_tree_ops.md)

## Verify・利用例

- [unit/balanced_trees](../../../../verify/unit/balanced_trees.cpp)
- [unit/pointer_structures](../../../../verify/unit/pointer_structures.cpp)
