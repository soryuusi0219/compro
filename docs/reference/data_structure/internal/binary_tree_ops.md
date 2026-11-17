# internal/binary_tree_ops

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/binary_tree_ops.hpp)

## 機能・前提条件

所有権を持たない、親付き二分木の接続操作。NodeはNode*型のpp/lp/rpを持ちます。Opsは静的なpush/pullを提供します。根判定は補助木の親子関係に基づき、LCTのpath-parentを通常の親子関係と区別します。ノードの確保・解放は呼出側が担当します。

## インターフェース

`BinaryTreeOps<Node,Ops>` は次を提供します。

- `is_root(p)`：pが補助木の根か。pは非null。
- `rotate(p)`：親との回転。pには補助木内の親が必要です。関連する祖先のタグは事前にpushしてください。接続が変わる2ノードを下からpullし、祖父はpullしません。path-parentは引き継ぎます。高さなど構造依存の情報を祖先へ再計算する処理はBackendが担当します。
- `locate(root,k,allow_end=false)`：論理位置kを含む `{Node*,ブロック内offset}`。Ops::size(p)とlocal_size(p)で重みを取得します。size(nullptr)=0、非負の重みを要求します。探索経路はpushします。

locateは非空木に対して `0 <= k < size(root)`。allow_end=trueならk=size(root)も許し、末尾ノードの末尾offsetを返します。呼出側はノード内の分割・挿入を担当します。

## 計算量

根判定・回転は O(1)、locateは木の高さHに対して O(H) 時間・O(1) 追加空間。Opsの各演算を O(1) とします。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [unit/balanced_trees](../../../../verify/unit/balanced_trees.cpp)
- [unit/pointer_structures](../../../../verify/unit/pointer_structures.cpp)
