# internal/link_cut_base

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/link_cut_base.hpp)

## 機能・前提条件

反転可能な補助木コアと集約Policyを利用するLCT。頂点番号は0始まり。固定長ノード配列を所有し、コピー禁止・move可能です。cloneは内部リンクを新配列へ付け替える独立複製です。

## インターフェース

`LinkCutBase<Ops,Auxiliary=SplayBase>`。Ops::Nodeはpp/lp/rp、value/lsum/rsum、label.indexを保持します。Opsはidentity/push/pull/toggleを提供し、作用付きではhas_action/action_type/applyも必要です。

Auxiliary<Node,Ops>はsplay(node)を実装し、補助木の根を越えて回転しないこと、path-parentを引き継ぐこと、祖先からpushすることが必須です。一般の列Backendをそのまま指定することはできません。

公開APIは既存のlinkcuttree/lazy_linkcuttreeと同じです。exposeで外した右の子のppはpath-parentとして残します。link/cut/evertなどの意味と不正操作の条件は各公開referenceを参照してください。

prod(i,j)/get_path(i,j)はiからjの向き。prod(i)はiから現在の根。2頂点操作は第2引数の頂点へevertします。LCAは現在の根に対する値です。

## 計算量

頂点数N。splay補助木の場合、通常操作は償却 O(log(N+1))、単発最悪 O(N)。構築・clone O(N)、get_pathはさらに出力長分。空間 O(N)。作業配列も管理オブジェクト内で保持します。

## 依存関係

- [splay_base.hpp](splay_base.md)

## Verify・利用例

- [unit/balanced_trees](../../../../verify/unit/balanced_trees.cpp)
- [unit/pointer_structures](../../../../verify/unit/pointer_structures.cpp)
